#include "main_window.h"

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

namespace crusty {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set window title and size
    setWindowTitle("CRUSTy-Qt");
    resize(800, 600);
    
    // Set up UI
    setupUi();
    
    // Create actions and menus
    createActions();
    createMenus();
    
    // Set up status bar
    statusBar()->addWidget(m_statusLabel = new QLabel(this));
    statusBar()->addPermanentWidget(m_progressBar = new QProgressBar(this));
    m_progressBar->setVisible(false);
    
    // Set up status message timer
    m_statusTimer.setSingleShot(true);
    connect(&m_statusTimer, &QTimer::timeout, [this]() {
        m_statusLabel->clear();
    });
    
    // Update UI state
    updateUiState();
}

void MainWindow::setupUi()
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create tab widget
    QTabWidget *tabWidget = new QTabWidget(centralWidget);
    mainLayout->addWidget(tabWidget);
    
    // Create encrypt tab
    QWidget *encryptTab = new QWidget(tabWidget);
    QVBoxLayout *encryptLayout = new QVBoxLayout(encryptTab);
    
    // Create encrypt form
    QFormLayout *encryptForm = new QFormLayout();
    
    // Add encrypt file input
    QHBoxLayout *encryptFileLayout = new QHBoxLayout();
    m_encryptFileEdit = new QLineEdit(encryptTab);
    m_encryptFileBrowseButton = new QPushButton("Browse...", encryptTab);
    encryptFileLayout->addWidget(m_encryptFileEdit);
    encryptFileLayout->addWidget(m_encryptFileBrowseButton);
    encryptForm->addRow("File to encrypt:", encryptFileLayout);
    
    // Add encrypt output input
    QHBoxLayout *encryptOutputLayout = new QHBoxLayout();
    m_encryptOutputEdit = new QLineEdit(encryptTab);
    m_encryptOutputBrowseButton = new QPushButton("Browse...", encryptTab);
    encryptOutputLayout->addWidget(m_encryptOutputEdit);
    encryptOutputLayout->addWidget(m_encryptOutputBrowseButton);
    encryptForm->addRow("Output file:", encryptOutputLayout);
    
    // Add encrypt password input
    m_encryptPasswordEdit = new QLineEdit(encryptTab);
    m_encryptPasswordEdit->setEchoMode(QLineEdit::Password);
    encryptForm->addRow("Password:", m_encryptPasswordEdit);
    
    // Add encrypt button
    m_encryptButton = new QPushButton("Encrypt", encryptTab);
    m_encryptButton->setEnabled(false);
    
    // Add form and button to layout
    encryptLayout->addLayout(encryptForm);
    encryptLayout->addWidget(m_encryptButton, 0, Qt::AlignRight);
    
    // Create decrypt tab
    QWidget *decryptTab = new QWidget(tabWidget);
    QVBoxLayout *decryptLayout = new QVBoxLayout(decryptTab);
    
    // Create decrypt form
    QFormLayout *decryptForm = new QFormLayout();
    
    // Add decrypt file input
    QHBoxLayout *decryptFileLayout = new QHBoxLayout();
    m_decryptFileEdit = new QLineEdit(decryptTab);
    m_decryptFileBrowseButton = new QPushButton("Browse...", decryptTab);
    decryptFileLayout->addWidget(m_decryptFileEdit);
    decryptFileLayout->addWidget(m_decryptFileBrowseButton);
    decryptForm->addRow("File to decrypt:", decryptFileLayout);
    
    // Add decrypt output input
    QHBoxLayout *decryptOutputLayout = new QHBoxLayout();
    m_decryptOutputEdit = new QLineEdit(decryptTab);
    m_decryptOutputBrowseButton = new QPushButton("Browse...", decryptTab);
    decryptOutputLayout->addWidget(m_decryptOutputEdit);
    decryptOutputLayout->addWidget(m_decryptOutputBrowseButton);
    decryptForm->addRow("Output file:", decryptOutputLayout);
    
    // Add decrypt password input
    m_decryptPasswordEdit = new QLineEdit(decryptTab);
    m_decryptPasswordEdit->setEchoMode(QLineEdit::Password);
    decryptForm->addRow("Password:", m_decryptPasswordEdit);
    
    // Add decrypt second factor input
    m_decryptSecondFactorEdit = new QLineEdit(decryptTab);
    decryptForm->addRow("Second factor (optional):", m_decryptSecondFactorEdit);
    
    // Add decrypt button
    m_decryptButton = new QPushButton("Decrypt", decryptTab);
    m_decryptButton->setEnabled(false);
    
    // Add form and button to layout
    decryptLayout->addLayout(decryptForm);
    decryptLayout->addWidget(m_decryptButton, 0, Qt::AlignRight);
    
    // Add tabs to tab widget
    tabWidget->addTab(encryptTab, "Encrypt");
    tabWidget->addTab(decryptTab, "Decrypt");
    
    // Set central widget
    setCentralWidget(centralWidget);
    
    // Connect signals and slots
    connect(m_encryptFileBrowseButton, &QPushButton::clicked, this, &MainWindow::selectEncryptFile);
    connect(m_encryptOutputBrowseButton, &QPushButton::clicked, this, &MainWindow::selectEncryptOutput);
    connect(m_decryptFileBrowseButton, &QPushButton::clicked, this, &MainWindow::selectDecryptFile);
    connect(m_decryptOutputBrowseButton, &QPushButton::clicked, this, &MainWindow::selectDecryptOutput);
    connect(m_encryptButton, &QPushButton::clicked, this, &MainWindow::encryptFile);
    connect(m_decryptButton, &QPushButton::clicked, this, &MainWindow::decryptFile);
    
    // Connect text changed signals to update UI state
    connect(m_encryptFileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encryptOutputEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encryptPasswordEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decryptFileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decryptOutputEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decryptPasswordEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
}

void MainWindow::createActions()
{
    // Create actions
}

void MainWindow::createMenus()
{
    // Create menus
    QMenu *fileMenu = menuBar()->addMenu("File");
    QMenu *helpMenu = menuBar()->addMenu("Help");
    
    // Add actions to menus
    QAction *exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    QAction *aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::updateUiState()
{
    // Enable encrypt button if all required fields are filled
    m_encryptButton->setEnabled(
        !m_encryptFileEdit->text().isEmpty() &&
        !m_encryptOutputEdit->text().isEmpty() &&
        !m_encryptPasswordEdit->text().isEmpty()
    );
    
    // Enable decrypt button if all required fields are filled
    m_decryptButton->setEnabled(
        !m_decryptFileEdit->text().isEmpty() &&
        !m_decryptOutputEdit->text().isEmpty() &&
        !m_decryptPasswordEdit->text().isEmpty()
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
    
    // Start timer to clear message after 5 seconds
    m_statusTimer.start(5000);
}

void MainWindow::selectEncryptFile()
{
    // Select file to encrypt
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select File to Encrypt",
        QDir::homePath(),
        "All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        m_encryptFileEdit->setText(filePath);
        
        // Set default output path
        if (m_encryptOutputEdit->text().isEmpty()) {
            m_encryptOutputEdit->setText(filePath + ".encrypted");
        }
    }
}

void MainWindow::selectEncryptOutput()
{
    // Select output file for encryption
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Select Output File",
        m_encryptOutputEdit->text().isEmpty() ?
            (m_encryptFileEdit->text().isEmpty() ? QDir::homePath() : m_encryptFileEdit->text() + ".encrypted") :
            m_encryptOutputEdit->text(),
        "Encrypted Files (*.encrypted);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        m_encryptOutputEdit->setText(filePath);
    }
}

void MainWindow::selectDecryptFile()
{
    // Select file to decrypt
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select File to Decrypt",
        QDir::homePath(),
        "Encrypted Files (*.encrypted);;All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        m_decryptFileEdit->setText(filePath);
        
        // Set default output path
        if (m_decryptOutputEdit->text().isEmpty()) {
            QString outputPath = filePath;
            if (outputPath.endsWith(".encrypted")) {
                outputPath.chop(10);
            } else {
                outputPath += ".decrypted";
            }
            m_decryptOutputEdit->setText(outputPath);
        }
    }
}

void MainWindow::selectDecryptOutput()
{
    // Select output file for decryption
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Select Output File",
        m_decryptOutputEdit->text().isEmpty() ?
            (m_decryptFileEdit->text().isEmpty() ? QDir::homePath() : m_decryptFileEdit->text() + ".decrypted") :
            m_decryptOutputEdit->text(),
        "All Files (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        m_decryptOutputEdit->setText(filePath);
    }
}

void MainWindow::encryptFile()
{
    // Get file paths and password
    QString sourcePath = m_encryptFileEdit->text();
    QString destPath = m_encryptOutputEdit->text();
    QString password = m_encryptPasswordEdit->text();
    
    // Check if source file exists
    if (!QFileInfo::exists(sourcePath)) {
        showStatusMessage("Source file does not exist", true);
        return;
    }
    
    // Check if destination file exists
    if (QFileInfo::exists(destPath)) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            "File Exists",
            "The output file already exists. Do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );
        
        if (result != QMessageBox::Yes) {
            return;
        }
        
        // Delete existing file
        QFile::remove(destPath);
    }
    
    // Disable UI during encryption
    setEnabled(false);
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    
    // Encrypt file in a separate thread
    std::thread([this, sourcePath, destPath, password]() {
        try {
            // Encrypt file
            m_encryptor.encryptFile(
                sourcePath.toStdString(),
                destPath.toStdString(),
                password.toStdString(),
                [this](float progress) {
                    // Update progress bar
                    QMetaObject::invokeMethod(m_progressBar, "setValue", Qt::QueuedConnection,
                        Q_ARG(int, static_cast<int>(progress * 100)));
                }
            );
            
            // Show success message
            QMetaObject::invokeMethod(this, "showStatusMessage", Qt::QueuedConnection,
                Q_ARG(QString, "File encrypted successfully"),
                Q_ARG(bool, false));
        } catch (const std::exception& e) {
            // Show error message
            QMetaObject::invokeMethod(this, "showStatusMessage", Qt::QueuedConnection,
                Q_ARG(QString, QString("Encryption failed: %1").arg(e.what())),
                Q_ARG(bool, true));
        }
        
        // Re-enable UI
        QMetaObject::invokeMethod(this, "setEnabled", Qt::QueuedConnection,
            Q_ARG(bool, true));
        QMetaObject::invokeMethod(m_progressBar, "setVisible", Qt::QueuedConnection,
            Q_ARG(bool, false));
    }).detach();
}

void MainWindow::decryptFile()
{
    // Get file paths and password
    QString sourcePath = m_decryptFileEdit->text();
    QString destPath = m_decryptOutputEdit->text();
    QString password = m_decryptPasswordEdit->text();
    QString secondFactor = m_decryptSecondFactorEdit->text();
    
    // Check if source file exists
    if (!QFileInfo::exists(sourcePath)) {
        showStatusMessage("Source file does not exist", true);
        return;
    }
    
    // Check if destination file exists
    if (QFileInfo::exists(destPath)) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            "File Exists",
            "The output file already exists. Do you want to overwrite it?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );
        
        if (result != QMessageBox::Yes) {
            return;
        }
        
        // Delete existing file
        QFile::remove(destPath);
    }
    
    // Disable UI during decryption
    setEnabled(false);
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    
    // Decrypt file in a separate thread
    std::thread([this, sourcePath, destPath, password, secondFactor]() {
        try {
            // Decrypt file
            m_encryptor.decryptFile(
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
                Q_ARG(QString, "File decrypted successfully"),
                Q_ARG(bool, false));
        } catch (const std::exception& e) {
            // Show error message
            QMetaObject::invokeMethod(this, "showStatusMessage", Qt::QueuedConnection,
                Q_ARG(QString, QString("Decryption failed: %1").arg(e.what())),
                Q_ARG(bool, true));
        }
        
        // Re-enable UI
        QMetaObject::invokeMethod(this, "setEnabled", Qt::QueuedConnection,
            Q_ARG(bool, true));
        QMetaObject::invokeMethod(m_progressBar, "setVisible", Qt::QueuedConnection,
            Q_ARG(bool, false));
    }).detach();
}

void MainWindow::showAbout()
{
    // Show about dialog
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
