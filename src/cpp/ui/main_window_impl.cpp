#include "main_window.h"
#include "main_window_constants.h"
#include "../core/audit_log.h"
#include "../core/path_utils.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QApplication>
#include <QHeaderView>
#include <QToolButton>
#include <QStyle>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QIcon>
#include <QFont>
#include <QFileIconProvider>

// Use the constants namespace
using namespace crusty::constants;

#include <thread>
#include <future>
#include <regex>

namespace crusty {

// Additional implementation methods for MainWindow class

void MainWindow::updateUiState()
{
    // Enable encrypt button if all required fields are filled
    m_encrypt.button->setEnabled(
        !m_encrypt.fileEdit->text().isEmpty() &&
        !m_encrypt.outputEdit->text().isEmpty() &&
        !m_encrypt.passwordEdit->text().isEmpty()
    );
    
    // Enable decrypt button if all required fields are filled
    m_decrypt.button->setEnabled(
        !m_decrypt.fileEdit->text().isEmpty() &&
        !m_decrypt.outputEdit->text().isEmpty() &&
        !m_decrypt.passwordEdit->text().isEmpty()
    );
    
    // Enable batch button if password is filled
    m_batch.button->setEnabled(
        !m_batch.passwordEdit->text().isEmpty() &&
        m_batch.fileModel->rowCount() > 0
    );
    
    // Update UI state based on input fields
}

void MainWindow::showStatusMessage(const QString& message, bool isError)
{
    // Set status message
    m_statusLabel->setText(message);
    
    // Set color based on error status
    QPalette palette = m_statusLabel->palette();
    palette.setColor(QPalette::WindowText, isError ? Qt::red : Qt::black);
    m_statusLabel->setPalette(palette);
    
    // Log the message
    if (isError) {
        LOG_ERROR(message.toStdString());
    } else {
        LOG_INFO(message.toStdString());
    }
    
    // Start timer to clear message after the specified duration
    m_statusTimer.start(STATUS_MESSAGE_DURATION_MS);
}

bool MainWindow::confirmFileOverwrite(const QString& filePath)
{
    if (!QFileInfo::exists(filePath)) {
        return true;
    }
    
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        "File Exists",
        "The output file already exists. Do you want to overwrite it?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (result != QMessageBox::Yes) {
        return false;
    }
    
    // Delete existing file
    return QFile::remove(filePath);
}

void MainWindow::processCryptoOperation(
    const std::function<void(
        const std::string&, 
        const std::string&, 
        const std::string&, 
        const std::string&, 
        const std::function<void(float)>&
    )>& operation,
    const QString& sourcePath,
    const QString& destPath,
    const QString& password,
    const QString& successMessage
)
{
    // Check if source file exists
    if (!QFileInfo::exists(sourcePath)) {
        showStatusMessage("Source file does not exist", true);
        return;
    }
    
    // Check if destination file exists and confirm overwrite
    if (!confirmFileOverwrite(destPath)) {
        return;
    }
    
    // Disable UI during operation
    setEnabled(false);
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    
    // Process operation in a separate thread
    std::thread([=]() {
        try {
            // Execute the operation
            operation(
                sourcePath.toStdString(),
                destPath.toStdString(),
                password.toStdString(),
                "", // No second factor
                [this](float progress) {
                    // Update progress bar
                    QMetaObject::invokeMethod(m_progressBar, "setValue", Qt::QueuedConnection,
                        Q_ARG(int, static_cast<int>(progress * 100)));
                }
            );
            
            // Show success message
            QMetaObject::invokeMethod(this, "showStatusMessage", Qt::QueuedConnection,
                Q_ARG(QString, successMessage),
                Q_ARG(bool, false));
        } catch (const std::exception& e) {
            // Show error message
            QMetaObject::invokeMethod(this, "showStatusMessage", Qt::QueuedConnection,
                Q_ARG(QString, QString("Operation failed: %1").arg(e.what())),
                Q_ARG(bool, true));
        }
        
        // Re-enable UI
        QMetaObject::invokeMethod(this, "setEnabled", Qt::QueuedConnection,
            Q_ARG(bool, true));
        QMetaObject::invokeMethod(m_progressBar, "setVisible", Qt::QueuedConnection,
            Q_ARG(bool, false));
    }).detach();
}

void MainWindow::encryptFile()
{
    processCryptoOperation(
        [this](const std::string& src, const std::string& dst, 
               const std::string& pwd, const std::string& secondFactor, 
               const std::function<void(float)>& progress) {
            m_encryptor.encryptFile(src, dst, pwd, progress);
        },
        m_encrypt.fileEdit->text(),
        m_encrypt.outputEdit->text(),
        m_encrypt.passwordEdit->text(),
        "File encrypted successfully"
    );
}

void MainWindow::decryptFile()
{
    processCryptoOperation(
        [this](const std::string& src, const std::string& dst, 
               const std::string& pwd, const std::string& secondFactor, 
               const std::function<void(float)>& progress) {
            // Note: secondFactor is ignored as the current implementation doesn't support it
            m_encryptor.decryptFile(src, dst, pwd, progress);
        },
        m_decrypt.fileEdit->text(),
        m_decrypt.outputEdit->text(),
        m_decrypt.passwordEdit->text(),
        "File decrypted successfully"
    );
}

void MainWindow::processBatch()
{
    // Not yet implemented
    showStatusMessage("Batch processing not yet implemented", true);
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Open File", QDir::homePath(), ALL_FILES_FILTER
    );
    
    if (!filePath.isEmpty()) {
        // Set file path in the appropriate tab based on current tab
        int currentTab = m_operationTabWidget->currentIndex();
        if (currentTab == 0) { // Encrypt tab
            m_encrypt.fileEdit->setText(filePath);
            if (m_encrypt.outputEdit->text().isEmpty()) {
                m_encrypt.outputEdit->setText(filePath + ENCRYPTED_EXTENSION);
            }
        } else if (currentTab == 1) { // Decrypt tab
            m_decrypt.fileEdit->setText(filePath);
            if (m_decrypt.outputEdit->text().isEmpty()) {
                QString outputPath = filePath;
                if (outputPath.endsWith(ENCRYPTED_EXTENSION)) {
                    outputPath.chop(strlen(ENCRYPTED_EXTENSION));
                } else {
                    outputPath += DECRYPTED_EXTENSION;
                }
                m_decrypt.outputEdit->setText(outputPath);
            }
        }
    }
}

void MainWindow::refreshFileList(const QString& directory)
{
    // Clear current model
    m_fileModel->clear();
    m_fileModel->setHorizontalHeaderLabels({"Name", "Size", "Date", "Path"});
    
    // If directory is provided, update current directory
    if (!directory.isEmpty()) {
        QFileInfo dirInfo(directory);
        if (dirInfo.exists() && dirInfo.isDir()) {
            m_currentDirectory = dirInfo.absoluteFilePath();
        } else {
            showStatusMessage("Invalid directory path: " + directory, true);
            return;
        }
    } else if (m_currentDirectory.isEmpty()) {
        // Default to home directory if no current directory
        m_currentDirectory = QDir::homePath();
    }
    
    // Update path edit
    if (m_pathEdit) {
        m_pathEdit->setText(m_currentDirectory);
    }
    
    // Add files to model
    QDir dir(m_currentDirectory);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QFileInfo& fileInfo : fileInfoList) {
        QList<QStandardItem*> row;
        
        // Name
        QStandardItem* nameItem = new QStandardItem(fileInfo.fileName());
        if (fileInfo.isDir()) {
            nameItem->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
        } else {
            // Use different icons based on file type
            QFileIconProvider iconProvider;
            nameItem->setIcon(iconProvider.icon(fileInfo));
        }
        row.append(nameItem);
        
        // Size
        QString sizeStr;
        if (fileInfo.isDir()) {
            sizeStr = "<DIR>";
        } else {
            qint64 size = fileInfo.size();
            if (size < 1024) {
                sizeStr = QString("%1 B").arg(size);
            } else if (size < 1024 * 1024) {
                sizeStr = QString("%1 KB").arg(size / 1024.0, 0, 'f', 2);
            } else if (size < 1024 * 1024 * 1024) {
                sizeStr = QString("%1 MB").arg(size / (1024.0 * 1024.0), 0, 'f', 2);
            } else {
                sizeStr = QString("%1 GB").arg(size / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
            }
        }
        row.append(new QStandardItem(sizeStr));
        
        // Date
        row.append(new QStandardItem(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
        
        // Path
        row.append(new QStandardItem(fileInfo.absoluteFilePath()));
        
        m_fileModel->appendRow(row);
    }
    
    // Sort by name by default
    m_fileTreeView->sortByColumn(FILE_NAME_COLUMN, Qt::AscendingOrder);
    
    // Update status bar
    showStatusMessage(QString("Loaded %1 items").arg(fileInfoList.size()), false);
}

void MainWindow::onFileSelected(const QModelIndex& index)
{
    // Get file path from the model
    QModelIndex sourceIndex = m_fileSortModel->mapToSource(index);
    QModelIndex pathIndex = m_fileModel->index(sourceIndex.row(), FILE_PATH_COLUMN);
    QString filePath = m_fileModel->data(pathIndex).toString();
    
    // Get file info
    QFileInfo fileInfo(filePath);
    
    // Set file path in the appropriate tab based on current tab
    int currentTab = m_operationTabWidget->currentIndex();
    if (currentTab == 0) { // Encrypt tab
        m_encrypt.fileEdit->setText(filePath);
        if (m_encrypt.outputEdit->text().isEmpty()) {
            m_encrypt.outputEdit->setText(filePath + ENCRYPTED_EXTENSION);
        }
    } else if (currentTab == 1) { // Decrypt tab
        m_decrypt.fileEdit->setText(filePath);
        if (m_decrypt.outputEdit->text().isEmpty()) {
            QString outputPath = filePath;
            if (outputPath.endsWith(ENCRYPTED_EXTENSION)) {
                outputPath.chop(strlen(ENCRYPTED_EXTENSION));
            } else {
                outputPath += DECRYPTED_EXTENSION;
            }
            m_decrypt.outputEdit->setText(outputPath);
        }
    }
    
    // Update file details panel
    QWidget* detailsPanel = m_mainSplitter->widget(1);
    if (detailsPanel) {
        // Find or create details layout
        QVBoxLayout* detailsLayout = nullptr;
        if (detailsPanel->layout()) {
            detailsLayout = qobject_cast<QVBoxLayout*>(detailsPanel->layout());
        } else {
            detailsLayout = new QVBoxLayout(detailsPanel);
        }
        
        // Clear existing widgets except the header
        while (detailsLayout->count() > 1) {
            QLayoutItem* item = detailsLayout->takeAt(1);
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        
        // Add file details
        QFormLayout* formLayout = new QFormLayout();
        formLayout->addRow("Name:", new QLabel(fileInfo.fileName()));
        formLayout->addRow("Type:", new QLabel(fileInfo.isDir() ? "Directory" : fileInfo.suffix().isEmpty() ? "File" : fileInfo.suffix().toUpper() + " File"));
        
        if (!fileInfo.isDir()) {
            // Format size
            qint64 size = fileInfo.size();
            QString sizeStr;
            if (size < 1024) {
                sizeStr = QString("%1 bytes").arg(size);
            } else if (size < 1024 * 1024) {
                sizeStr = QString("%1 KB (%2 bytes)").arg(size / 1024.0, 0, 'f', 2).arg(size);
            } else if (size < 1024 * 1024 * 1024) {
                sizeStr = QString("%1 MB (%2 bytes)").arg(size / (1024.0 * 1024.0), 0, 'f', 2).arg(size);
            } else {
                sizeStr = QString("%1 GB (%2 bytes)").arg(size / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2).arg(size);
            }
            formLayout->addRow("Size:", new QLabel(sizeStr));
        }
        
        formLayout->addRow("Created:", new QLabel(fileInfo.birthTime().toString("yyyy-MM-dd hh:mm:ss")));
        formLayout->addRow("Modified:", new QLabel(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
        formLayout->addRow("Path:", new QLabel(fileInfo.absolutePath()));
        
        // Add actions section
        QLabel* actionsLabel = new QLabel("Actions");
        actionsLabel->setProperty("class", "section-header");
        
        QHBoxLayout* actionsLayout = new QHBoxLayout();
        
        QPushButton* encryptButton = new QPushButton("Encrypt");
        encryptButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
        connect(encryptButton, &QPushButton::clicked, this, [this, filePath]() {
            m_operationTabWidget->setCurrentIndex(0); // Switch to encrypt tab
            m_encrypt.fileEdit->setText(filePath);
            if (m_encrypt.outputEdit->text().isEmpty()) {
                m_encrypt.outputEdit->setText(filePath + ENCRYPTED_EXTENSION);
            }
        });
        
        QPushButton* decryptButton = new QPushButton("Decrypt");
        decryptButton->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
        connect(decryptButton, &QPushButton::clicked, this, [this, filePath]() {
            m_operationTabWidget->setCurrentIndex(1); // Switch to decrypt tab
            m_decrypt.fileEdit->setText(filePath);
            if (m_decrypt.outputEdit->text().isEmpty()) {
                QString outputPath = filePath;
                if (outputPath.endsWith(ENCRYPTED_EXTENSION)) {
                    outputPath.chop(strlen(ENCRYPTED_EXTENSION));
                } else {
                    outputPath += DECRYPTED_EXTENSION;
                }
                m_decrypt.outputEdit->setText(outputPath);
            }
        });
        
        actionsLayout->addWidget(encryptButton);
        actionsLayout->addWidget(decryptButton);
        
        // Add to details layout
        detailsLayout->addLayout(formLayout);
        detailsLayout->addSpacing(20);
        detailsLayout->addWidget(actionsLabel);
        detailsLayout->addLayout(actionsLayout);
        detailsLayout->addStretch();
    }
}

void MainWindow::showSettings()
{
    // Not yet implemented
    showStatusMessage("Settings dialog not yet implemented", true);
}

void MainWindow::showKeyManagement()
{
    // Not yet implemented
    showStatusMessage("Key management not yet implemented", true);
}

void MainWindow::showDeviceManagement()
{
    // Switch to device tab
    m_operationTabWidget->setCurrentIndex(3);
    
    // Not yet implemented
    showStatusMessage("Device management not yet implemented", true);
}

// 2FA functions are not implemented in this version

void MainWindow::showAbout()
{
    QMessageBox::about(
        this,
        "About CRUSTy-Core",
        "<h3>CRUSTy-Core</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A secure file encryption application using AES-256-GCM encryption.</p>"
        "<p>Built with Qt and Rust.</p>"
        "<p>&copy; 2025 CRUSTy Team</p>"
    );
}


void MainWindow::onFileDoubleClicked(const QModelIndex& index)
{
    // Get file path from the model
    QModelIndex sourceIndex = m_fileSortModel->mapToSource(index);
    QModelIndex pathIndex = m_fileModel->index(sourceIndex.row(), FILE_PATH_COLUMN);
    QString filePath = m_fileModel->data(pathIndex).toString();
    
    // Get file info
    QFileInfo fileInfo(filePath);
    
    // If it's a directory, navigate to it
    if (fileInfo.isDir()) {
        refreshFileList(filePath);
    } else {
        // If it's a file, handle based on current tab
        int currentTab = m_operationTabWidget->currentIndex();
        if (currentTab == 0) { // Encrypt tab
            m_encrypt.fileEdit->setText(filePath);
            if (m_encrypt.outputEdit->text().isEmpty()) {
                m_encrypt.outputEdit->setText(filePath + ENCRYPTED_EXTENSION);
            }
        } else if (currentTab == 1) { // Decrypt tab
            m_decrypt.fileEdit->setText(filePath);
            if (m_decrypt.outputEdit->text().isEmpty()) {
                QString outputPath = filePath;
                if (outputPath.endsWith(ENCRYPTED_EXTENSION)) {
                    outputPath.chop(strlen(ENCRYPTED_EXTENSION));
                } else {
                    outputPath += DECRYPTED_EXTENSION;
                }
                m_decrypt.outputEdit->setText(outputPath);
            }
        }
    }
}

void MainWindow::navigateUp()
{
    if (m_currentDirectory.isEmpty()) {
        return;
    }
    
    QDir currentDir(m_currentDirectory);
    if (currentDir.cdUp()) {
        refreshFileList(currentDir.absolutePath());
    }
}

} // namespace crusty
