#include "main_window.h"
#include "../core/audit_log.h"
#include "../core/path_utils.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <thread>
#include <future>

// Constants
namespace {
    // UI Constants
    constexpr int STATUS_MESSAGE_DURATION_MS = 5000;
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    
    // File Extensions
    constexpr const char* ENCRYPTED_EXTENSION = ".encrypted";
    constexpr const char* DECRYPTED_EXTENSION = ".decrypted";
    
    // Filter Strings
    constexpr const char* ALL_FILES_FILTER = "All Files (*.*)";
    constexpr const char* ENCRYPTED_FILES_FILTER = "Encrypted Files (*.encrypted);;All Files (*.*)";
}

namespace crusty {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CRUSTy-Qt");
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    setupUi();
    setupMenus();
    setupFileSelectors();
    
    // Set up status bar
    statusBar()->addWidget(m_statusLabel = new QLabel(this));
    statusBar()->addPermanentWidget(m_progressBar = new QProgressBar(this));
    m_progressBar->setVisible(false);
    
    // Set up status message timer
    m_statusTimer.setSingleShot(true);
    connect(&m_statusTimer, &QTimer::timeout, [this]() {
        m_statusLabel->clear();
    });
    
    updateUiState();
}

void MainWindow::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    QTabWidget* tabWidget = new QTabWidget(centralWidget);
    
    // Add encrypt and decrypt tabs
    tabWidget->addTab(createEncryptTab(), "Encrypt");
    tabWidget->addTab(createDecryptTab(), "Decrypt");
    
    mainLayout->addWidget(tabWidget);
    setCentralWidget(centralWidget);
    
    // Connect text changed signals to update UI state
    connect(m_encrypt.fileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encrypt.outputEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encrypt.passwordEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decrypt.fileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decrypt.outputEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decrypt.passwordEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
}

QWidget* MainWindow::createEncryptTab()
{
    QWidget* encryptTab = new QWidget();
    QVBoxLayout* encryptLayout = new QVBoxLayout(encryptTab);
    
    // Create encrypt form
    QFormLayout* encryptForm = new QFormLayout();
    
    // Add encrypt file input
    QHBoxLayout* encryptFileLayout = new QHBoxLayout();
    m_encrypt.fileEdit = new QLineEdit(encryptTab);
    QPushButton* encryptFileBrowseButton = new QPushButton("Browse...", encryptTab);
    encryptFileLayout->addWidget(m_encrypt.fileEdit);
    encryptFileLayout->addWidget(encryptFileBrowseButton);
    encryptForm->addRow("File to encrypt:", encryptFileLayout);
    
    // Add encrypt output input
    QHBoxLayout* encryptOutputLayout = new QHBoxLayout();
    m_encrypt.outputEdit = new QLineEdit(encryptTab);
    QPushButton* encryptOutputBrowseButton = new QPushButton("Browse...", encryptTab);
    encryptOutputLayout->addWidget(m_encrypt.outputEdit);
    encryptOutputLayout->addWidget(encryptOutputBrowseButton);
    encryptForm->addRow("Output file:", encryptOutputLayout);
    
    // Add encrypt password input
    m_encrypt.passwordEdit = new QLineEdit(encryptTab);
    m_encrypt.passwordEdit->setEchoMode(QLineEdit::Password);
    encryptForm->addRow("Password:", m_encrypt.passwordEdit);
    
    // Add encrypt button
    m_encrypt.button = new QPushButton("Encrypt", encryptTab);
    m_encrypt.button->setEnabled(false);
    
    // Add form and button to layout
    encryptLayout->addLayout(encryptForm);
    encryptLayout->addWidget(m_encrypt.button, 0, Qt::AlignRight);
    
    // Connect button signal
    connect(m_encrypt.button, &QPushButton::clicked, this, &MainWindow::encryptFile);
    
    return encryptTab;
}

QWidget* MainWindow::createDecryptTab()
{
    QWidget* decryptTab = new QWidget();
    QVBoxLayout* decryptLayout = new QVBoxLayout(decryptTab);
    
    // Create decrypt form
    QFormLayout* decryptForm = new QFormLayout();
    
    // Add decrypt file input
    QHBoxLayout* decryptFileLayout = new QHBoxLayout();
    m_decrypt.fileEdit = new QLineEdit(decryptTab);
    QPushButton* decryptFileBrowseButton = new QPushButton("Browse...", decryptTab);
    decryptFileLayout->addWidget(m_decrypt.fileEdit);
    decryptFileLayout->addWidget(decryptFileBrowseButton);
    decryptForm->addRow("File to decrypt:", decryptFileLayout);
    
    // Add decrypt output input
    QHBoxLayout* decryptOutputLayout = new QHBoxLayout();
    m_decrypt.outputEdit = new QLineEdit(decryptTab);
    QPushButton* decryptOutputBrowseButton = new QPushButton("Browse...", decryptTab);
    decryptOutputLayout->addWidget(m_decrypt.outputEdit);
    decryptOutputLayout->addWidget(decryptOutputBrowseButton);
    decryptForm->addRow("Output file:", decryptOutputLayout);
    
    // Add decrypt password input
    m_decrypt.passwordEdit = new QLineEdit(decryptTab);
    m_decrypt.passwordEdit->setEchoMode(QLineEdit::Password);
    decryptForm->addRow("Password:", m_decrypt.passwordEdit);
    
    // Add decrypt second factor input
    m_decrypt.secondFactorEdit = new QLineEdit(decryptTab);
    decryptForm->addRow("Second factor (optional):", m_decrypt.secondFactorEdit);
    
    // Add decrypt button
    m_decrypt.button = new QPushButton("Decrypt", decryptTab);
    m_decrypt.button->setEnabled(false);
    
    // Add form and button to layout
    decryptLayout->addLayout(decryptForm);
    decryptLayout->addWidget(m_decrypt.button, 0, Qt::AlignRight);
    
    // Connect button signal
    connect(m_decrypt.button, &QPushButton::clicked, this, &MainWindow::decryptFile);
    
    return decryptTab;
}

void MainWindow::setupFileSelectors()
{
    // Find encrypt tab browse buttons
    auto encryptFileBrowseButton = m_encrypt.fileEdit->parentWidget()->findChild<QPushButton*>();
    auto encryptOutputBrowseButton = m_encrypt.outputEdit->parentWidget()->findChild<QPushButton*>();
    
    // Find decrypt tab browse buttons
    auto decryptFileBrowseButton = m_decrypt.fileEdit->parentWidget()->findChild<QPushButton*>();
    auto decryptOutputBrowseButton = m_decrypt.outputEdit->parentWidget()->findChild<QPushButton*>();
    
    // Connect encrypt file browser
    connect(encryptFileBrowseButton, &QPushButton::clicked, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Select File to Encrypt", QDir::homePath(), ALL_FILES_FILTER
        );
        
        if (!filePath.isEmpty()) {
            m_encrypt.fileEdit->setText(filePath);
            
            // Set default output path if empty
            if (m_encrypt.outputEdit->text().isEmpty()) {
                m_encrypt.outputEdit->setText(filePath + ENCRYPTED_EXTENSION);
            }
        }
    });
    
    // Connect encrypt output browser
    connect(encryptOutputBrowseButton, &QPushButton::clicked, [this]() {
        QString defaultPath = m_encrypt.outputEdit->text();
        if (defaultPath.isEmpty() && !m_encrypt.fileEdit->text().isEmpty()) {
            defaultPath = m_encrypt.fileEdit->text() + ENCRYPTED_EXTENSION;
        }
        
        QString filePath = QFileDialog::getSaveFileName(
            this, "Select Output File", 
            defaultPath.isEmpty() ? QDir::homePath() : defaultPath,
            ENCRYPTED_FILES_FILTER
        );
        
        if (!filePath.isEmpty()) {
            m_encrypt.outputEdit->setText(filePath);
        }
    });
    
    // Connect decrypt file browser
    connect(decryptFileBrowseButton, &QPushButton::clicked, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Select File to Decrypt", QDir::homePath(),
            ENCRYPTED_FILES_FILTER
        );
        
        if (!filePath.isEmpty()) {
            m_decrypt.fileEdit->setText(filePath);
            
            // Set default output path if empty
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
    });
    
    // Connect decrypt output browser
    connect(decryptOutputBrowseButton, &QPushButton::clicked, [this]() {
        QString defaultPath = m_decrypt.outputEdit->text();
        if (defaultPath.isEmpty() && !m_decrypt.fileEdit->text().isEmpty()) {
            QString sourcePath = m_decrypt.fileEdit->text();
            if (sourcePath.endsWith(ENCRYPTED_EXTENSION)) {
                defaultPath = sourcePath;
                defaultPath.chop(strlen(ENCRYPTED_EXTENSION));
            } else {
                defaultPath = sourcePath + DECRYPTED_EXTENSION;
            }
        }
        
        QString filePath = QFileDialog::getSaveFileName(
            this, "Select Output File", 
            defaultPath.isEmpty() ? QDir::homePath() : defaultPath,
            ALL_FILES_FILTER
        );
        
        if (!filePath.isEmpty()) {
            m_decrypt.outputEdit->setText(filePath);
        }
    });
}

void MainWindow::setupMenus()
{
    // Create menus
    QMenu* fileMenu = menuBar()->addMenu("File");
    QMenu* helpMenu = menuBar()->addMenu("Help");
    
    // Add actions to menus
    QAction* exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    QAction* aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

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
    const QString& secondFactor,
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
                secondFactor.toStdString(),
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
               const std::string& pwd, const std::string&, 
               const std::function<void(float)>& progress) {
            m_encryptor.encryptFile(src, dst, pwd, progress);
        },
        m_encrypt.fileEdit->text(),
        m_encrypt.outputEdit->text(),
        m_encrypt.passwordEdit->text(),
        "",
        "File encrypted successfully"
    );
}

void MainWindow::decryptFile()
{
    processCryptoOperation(
        [this](const std::string& src, const std::string& dst, 
               const std::string& pwd, const std::string& secondFactor, 
               const std::function<void(float)>& progress) {
            m_encryptor.decryptFile(src, dst, pwd, secondFactor, progress);
        },
        m_decrypt.fileEdit->text(),
        m_decrypt.outputEdit->text(),
        m_decrypt.passwordEdit->text(),
        m_decrypt.secondFactorEdit->text(),
        "File decrypted successfully"
    );
}

void MainWindow::showAbout()
{
    QMessageBox::about(
        this,
        "About CRUSTy-Qt",
        "<h3>CRUSTy-Qt</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A secure file encryption application using AES-256-GCM encryption.</p>"
        "<p>Built with Qt and Rust.</p>"
        "<p>&copy; 2025 CRUSTy Team</p>"
    );
}

} // namespace crusty
