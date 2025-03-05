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

#include <thread>
#include <future>
#include <regex>

// Use the constants namespace
using namespace crusty::constants;

namespace crusty {

// PasswordStrengthMeter Implementation
PasswordStrengthMeter::PasswordStrengthMeter(QWidget* parent)
    : QProgressBar(parent)
{
    setTextVisible(true);
    setRange(0, 100);
    setValue(0);
    setFormat("%p%");
    setMinimumWidth(150);
}

void PasswordStrengthMeter::updateStrength(const QString& password)
{
    int strength = calculateStrength(password);
    setValue(strength);
    
    // Update color based on strength
    QString styleSheet;
    if (strength < 40) {
        styleSheet = "QProgressBar::chunk { background-color: #FF5252; }";
        setFormat("Weak: %p%");
    } else if (strength < 70) {
        styleSheet = "QProgressBar::chunk { background-color: #FFD740; }";
        setFormat("Medium: %p%");
    } else {
        styleSheet = "QProgressBar::chunk { background-color: #00C853; }";
        setFormat("Strong: %p%");
    }
    
    setStyleSheet(styleSheet);
}

int PasswordStrengthMeter::calculateStrength(const QString& password)
{
    if (password.isEmpty()) {
        return 0;
    }
    
    int score = 0;
    
    // Length check
    score += qMin(password.length() * 4, 40);
    
    // Character variety checks
    bool hasLower = false;
    bool hasUpper = false;
    bool hasDigit = false;
    bool hasSpecial = false;
    
    for (const QChar& c : password) {
        if (c.isLower()) hasLower = true;
        else if (c.isUpper()) hasUpper = true;
        else if (c.isDigit()) hasDigit = true;
        else hasSpecial = true;
    }
    
    if (hasLower) score += 10;
    if (hasUpper) score += 10;
    if (hasDigit) score += 10;
    if (hasSpecial) score += 15;
    
    // Bonus for combination of character types
    int typesCount = hasLower + hasUpper + hasDigit + hasSpecial;
    score += (typesCount - 1) * 5;
    
    // Penalize repeated characters
    std::regex repeatedChars("(.)\\1{2,}");
    std::string stdPassword = password.toStdString();
    auto words_begin = std::sregex_iterator(stdPassword.begin(), stdPassword.end(), repeatedChars);
    auto words_end = std::sregex_iterator();
    score -= std::distance(words_begin, words_end) * 5;
    
    // Ensure score is between 0 and 100
    return qBound(0, score, 100);
}

// MainWindow Implementation
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CRUSTy-Core");
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    applyStyleSheet();
    setupUi();
    setupToolBar();
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

void MainWindow::applyStyleSheet()
{
    // Load style sheet from resources
    QFile styleFile(":/ui/styles.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        LOG_ERROR("Failed to load style sheet");
    }
}

void MainWindow::setupUi()
{
    // Create central widget with vertical layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setCentralWidget(centralWidget);
    
    // Create operation tab widget
    m_operationTabWidget = new QTabWidget(this);
    
    // Add tabs
    m_operationTabWidget->addTab(createEncryptTab(), "Encrypt");
    m_operationTabWidget->addTab(createDecryptTab(), "Decrypt");
    m_operationTabWidget->addTab(createBatchTab(), "Batch Processing");
    m_operationTabWidget->addTab(createDeviceTab(), "Embedded Devices");
    
    // Create main splitter (below the tabs)
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Create file browser panel
    QWidget* fileBrowserPanel = createFileBrowserPanel();
    m_mainSplitter->addWidget(fileBrowserPanel);
    
    // Create details panel (placeholder for now)
    QWidget* detailsPanel = new QWidget(this);
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsPanel);
    QLabel* detailsLabel = new QLabel("File Details", detailsPanel);
    detailsLabel->setProperty("class", "section-header");
    detailsLayout->addWidget(detailsLabel);
    detailsLayout->addStretch();
    m_mainSplitter->addWidget(detailsPanel);
    
    // Set splitter sizes
    QList<int> sizes;
    sizes << 700 << 300; // Reversed ratio from before to make file list the main component
    m_mainSplitter->setSizes(sizes);
    
    // Add tab widget and splitter to main layout
    mainLayout->addWidget(m_operationTabWidget);
    mainLayout->addWidget(m_mainSplitter, 1); // Give the splitter a stretch factor of 1
    
    // Connect signals
    connect(m_encrypt.fileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encrypt.outputEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encrypt.passwordEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_encrypt.passwordEdit, &QLineEdit::textChanged, m_encrypt.strengthMeter, &PasswordStrengthMeter::updateStrength);
    connect(m_decrypt.fileEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decrypt.outputEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_decrypt.passwordEdit, &QLineEdit::textChanged, this, &MainWindow::updateUiState);
    connect(m_fileTreeView, &QTreeView::clicked, this, &MainWindow::onFileSelected);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("Main Toolbar");
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(24, 24));
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    // Create actions
    m_openAction = m_toolBar->addAction("Open");
    m_openAction->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    m_openAction->setToolTip("Open File or Directory");
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    m_toolBar->addSeparator();
    
    m_encryptAction = m_toolBar->addAction("Encrypt");
    m_encryptAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_encryptAction->setToolTip("Encrypt File");
    connect(m_encryptAction, &QAction::triggered, [this]() {
        m_operationTabWidget->setCurrentIndex(0);
    });
    
    m_decryptAction = m_toolBar->addAction("Decrypt");
    m_decryptAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    m_decryptAction->setToolTip("Decrypt File");
    connect(m_decryptAction, &QAction::triggered, [this]() {
        m_operationTabWidget->setCurrentIndex(1);
    });
    
    m_batchAction = m_toolBar->addAction("Batch");
    m_batchAction->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    m_batchAction->setToolTip("Batch Processing");
    connect(m_batchAction, &QAction::triggered, [this]() {
        m_operationTabWidget->setCurrentIndex(2);
    });
    
    m_toolBar->addSeparator();
    
    m_deviceAction = m_toolBar->addAction("Devices");
    m_deviceAction->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_deviceAction->setToolTip("Manage Embedded Devices");
    connect(m_deviceAction, &QAction::triggered, this, &MainWindow::showDeviceManagement);
    
    m_keyAction = m_toolBar->addAction("Keys");
    m_keyAction->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    m_keyAction->setToolTip("Key Management");
    connect(m_keyAction, &QAction::triggered, this, &MainWindow::showKeyManagement);
    
    m_toolBar->addSeparator();
    
    m_settingsAction = m_toolBar->addAction("Settings");
    m_settingsAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    m_settingsAction->setToolTip("Settings");
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettings);
    
    // Add a stretch spacer to right-align the next actions
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolBar->addWidget(spacer);
    
    // Add a refresh action on the right side
    QPushButton* refreshButton = new QPushButton("Refresh");
    refreshButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshFileList);
    m_toolBar->addWidget(refreshButton);
}

void MainWindow::setupMenus()
{
    // Create menus
    QMenu* fileMenu = menuBar()->addMenu("File");
    QMenu* toolsMenu = menuBar()->addMenu("Tools");
    QMenu* settingsMenu = menuBar()->addMenu("Settings");
    QMenu* helpMenu = menuBar()->addMenu("Help");
    
    // File menu
    fileMenu->addAction(m_openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_encryptAction);
    fileMenu->addAction(m_decryptAction);
    fileMenu->addAction(m_batchAction);
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // Tools menu
    toolsMenu->addAction(m_deviceAction);
    toolsMenu->addAction(m_keyAction);
    
    // Settings menu
    QAction* settingsAction = settingsMenu->addAction("Preferences");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettings);
    
    // Help menu
    QAction* aboutAction = helpMenu->addAction("About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

QWidget* MainWindow::createFileBrowserPanel()
{
    QWidget* panel = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create file model
    m_fileModel = new QStandardItemModel(this);
    m_fileModel->setHorizontalHeaderLabels({"Name", "Size", "Date", "Path"});
    
    // Create sort proxy model
    m_fileSortModel = new QSortFilterProxyModel(this);
    m_fileSortModel->setSourceModel(m_fileModel);
    m_fileSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    
    // Create tree view
    m_fileTreeView = new QTreeView(this);
    m_fileTreeView->setModel(m_fileSortModel);
    m_fileTreeView->setSortingEnabled(true);
    m_fileTreeView->setAlternatingRowColors(true);
    m_fileTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fileTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fileTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fileTreeView->setColumnWidth(FILE_NAME_COLUMN, 250);
    m_fileTreeView->setColumnWidth(FILE_SIZE_COLUMN, 100);
    m_fileTreeView->setColumnWidth(FILE_DATE_COLUMN, 150);
    m_fileTreeView->setColumnHidden(FILE_PATH_COLUMN, true);
    m_fileTreeView->header()->setSectionResizeMode(FILE_NAME_COLUMN, QHeaderView::Stretch);
    m_fileTreeView->header()->setSectionResizeMode(FILE_SIZE_COLUMN, QHeaderView::Fixed);
    m_fileTreeView->header()->setSectionResizeMode(FILE_DATE_COLUMN, QHeaderView::Fixed);
    
    // Create address bar for file browser
    QHBoxLayout* addressBarLayout = new QHBoxLayout();
    addressBarLayout->setContentsMargins(5, 5, 5, 5);
    
    QLabel* addressLabel = new QLabel("Address:", this);
    QPushButton* upButton = new QPushButton(this);
    upButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    upButton->setToolTip("Go Up One Level");
    upButton->setFixedSize(24, 24);
    
    QLineEdit* pathEdit = new QLineEdit(this);
    pathEdit->setReadOnly(true);
    
    addressBarLayout->addWidget(addressLabel);
    addressBarLayout->addWidget(upButton);
    addressBarLayout->addWidget(pathEdit, 1);
    
    // Create a header for the file list
    QLabel* fileListHeader = new QLabel("Files", this);
    fileListHeader->setProperty("class", "section-header");
    
    // Add to layout
    layout->addLayout(addressBarLayout);
    layout->addWidget(fileListHeader);
    layout->addWidget(m_fileTreeView);
    
    // Connect signals
    connect(upButton, &QPushButton::clicked, [this, pathEdit]() {
        QDir currentDir(pathEdit->text());
        if (currentDir.cdUp()) {
            pathEdit->setText(currentDir.absolutePath());
            refreshFileList();
        }
    });
    
    // Initial file list
    refreshFileList();
    
    return panel;
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
    
    // Add password strength meter
    m_encrypt.strengthMeter = new PasswordStrengthMeter(encryptTab);
    encryptForm->addRow("Password strength:", m_encrypt.strengthMeter);
    
    // Add 2FA checkbox
    QHBoxLayout* twoFactorLayout = new QHBoxLayout();
    m_encrypt.twoFactorCheckBox = new QCheckBox("Enable Two-Factor Authentication", encryptTab);
    m_encrypt.twoFactorConfigButton = new QPushButton("Configure...", encryptTab);
    m_encrypt.twoFactorConfigButton->setEnabled(false);
    twoFactorLayout->addWidget(m_encrypt.twoFactorCheckBox);
    twoFactorLayout->addWidget(m_encrypt.twoFactorConfigButton);
    encryptForm->addRow("", twoFactorLayout);
    
    // Add encrypt button
    m_encrypt.button = new QPushButton("Encrypt", encryptTab);
    m_encrypt.button->setEnabled(false);
    m_encrypt.button->setProperty("class", "action-button");
    
    // Add form and button to layout
    encryptLayout->addLayout(encryptForm);
    encryptLayout->addStretch();
    encryptLayout->addWidget(m_encrypt.button, 0, Qt::AlignRight);
    
    // Connect signals
    connect(m_encrypt.button, &QPushButton::clicked, this, &MainWindow::encryptFile);
    connect(m_encrypt.twoFactorCheckBox, &QCheckBox::toggled, this, &MainWindow::toggle2FA);
    connect(m_encrypt.twoFactorConfigButton, &QPushButton::clicked, this, &MainWindow::configure2FA);
    
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
    decryptForm->addRow("Second factor (if enabled):", m_decrypt.secondFactorEdit);
    
    // Add decrypt button
    m_decrypt.button = new QPushButton("Decrypt", decryptTab);
    m_decrypt.button->setEnabled(false);
    m_decrypt.button->setProperty("class", "action-button");
    
    // Add form and button to layout
    decryptLayout->addLayout(decryptForm);
    decryptLayout->addStretch();
    decryptLayout->addWidget(m_decrypt.button, 0, Qt::AlignRight);
    
    // Connect signals
    connect(m_decrypt.button, &QPushButton::clicked, this, &MainWindow::decryptFile);
    
    return decryptTab;
}

QWidget* MainWindow::createBatchTab()
{
    QWidget* batchTab = new QWidget();
    QVBoxLayout* batchLayout = new QVBoxLayout(batchTab);
    
    // Create batch file list
    QGroupBox* fileListGroup = new QGroupBox("Files to Process", batchTab);
    QVBoxLayout* fileListLayout = new QVBoxLayout(fileListGroup);
    
    m_batch.fileModel = new QStandardItemModel(this);
    m_batch.fileModel->setHorizontalHeaderLabels({"File", "Output", "Status"});
    
    m_batch.fileTable = new QTableView(batchTab);
    m_batch.fileTable->setModel(m_batch.fileModel);
    m_batch.fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_batch.fileTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_batch.fileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_batch.fileTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_batch.fileTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_batch.fileTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_batch.fileTable->setColumnWidth(2, 100);
    
    QHBoxLayout* fileButtonLayout = new QHBoxLayout();
    m_batch.addButton = new QPushButton("Add Files...", batchTab);
    m_batch.removeButton = new QPushButton("Remove", batchTab);
    fileButtonLayout->addWidget(m_batch.addButton);
    fileButtonLayout->addWidget(m_batch.removeButton);
    fileButtonLayout->addStretch();
    
    fileListLayout->addWidget(m_batch.fileTable);
    fileListLayout->addLayout(fileButtonLayout);
    
    // Create batch settings
    QGroupBox* settingsGroup = new QGroupBox("Batch Settings", batchTab);
    QFormLayout* settingsLayout = new QFormLayout(settingsGroup);
    
    m_batch.operationCombo = new QComboBox(batchTab);
    m_batch.operationCombo->addItem("Encrypt");
    m_batch.operationCombo->addItem("Decrypt");
    settingsLayout->addRow("Operation:", m_batch.operationCombo);
    
    m_batch.passwordEdit = new QLineEdit(batchTab);
    m_batch.passwordEdit->setEchoMode(QLineEdit::Password);
    settingsLayout->addRow("Password:", m_batch.passwordEdit);
    
    m_batch.secondFactorEdit = new QLineEdit(batchTab);
    settingsLayout->addRow("Second factor (if needed):", m_batch.secondFactorEdit);
    
    // Add batch button
    m_batch.button = new QPushButton("Process Batch", batchTab);
    m_batch.button->setEnabled(false);
    m_batch.button->setProperty("class", "action-button");
    
    // Add to layout
    batchLayout->addWidget(fileListGroup);
    batchLayout->addWidget(settingsGroup);
    batchLayout->addStretch();
    batchLayout->addWidget(m_batch.button, 0, Qt::AlignRight);
    
    // Connect signals
    connect(m_batch.button, &QPushButton::clicked, this, &MainWindow::processBatch);
    
    return batchTab;
}

QWidget* MainWindow::createDeviceTab()
{
    QWidget* deviceTab = new QWidget();
    QVBoxLayout* deviceLayout = new QVBoxLayout(deviceTab);
    
    // Create device list
    QGroupBox* deviceListGroup = new QGroupBox("Embedded Devices", deviceTab);
    QVBoxLayout* deviceListLayout = new QVBoxLayout(deviceListGroup);
    
    m_device.deviceModel = new QStandardItemModel(this);
    m_device.deviceModel->setHorizontalHeaderLabels({"Device", "Type", "Status", "ID"});
    
    m_device.deviceTable = new QTableView(deviceTab);
    m_device.deviceTable->setModel(m_device.deviceModel);
    m_device.deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_device.deviceTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_device.deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_device.deviceTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_device.deviceTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_device.deviceTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_device.deviceTable->setColumnHidden(DEVICE_ID_COLUMN, true);
    m_device.deviceTable->setColumnWidth(1, 150);
    m_device.deviceTable->setColumnWidth(2, 100);
    
    QHBoxLayout* deviceButtonLayout = new QHBoxLayout();
    m_device.refreshButton = new QPushButton("Refresh", deviceTab);
    m_device.connectButton = new QPushButton("Connect", deviceTab);
    m_device.installButton = new QPushButton("Install Firmware", deviceTab);
    deviceButtonLayout->addWidget(m_device.refreshButton);
    deviceButtonLayout->addWidget(m_device.connectButton);
    deviceButtonLayout->addWidget(m_device.installButton);
    deviceButtonLayout->addStretch();
    
    deviceListLayout->addWidget(m_device.deviceTable);
    deviceListLayout->addLayout(deviceButtonLayout);
    
    // Create device operations
    QGroupBox* operationsGroup = new QGroupBox("Device Operations", deviceTab);
    QVBoxLayout* operationsLayout = new QVBoxLayout(operationsGroup);
    
    QLabel* infoLabel = new QLabel("Select a device to perform operations.", deviceTab);
    m_device.statusLabel = new QLabel("No device connected", deviceTab);
    m_device.statusLabel->setProperty("class", "device-disconnected");
    
    operationsLayout->addWidget(infoLabel);
    operationsLayout->addWidget(m_device.statusLabel);
    operationsLayout->addStretch();
    
    // Add to layout
    deviceLayout->addWidget(deviceListGroup);
    deviceLayout->addWidget(operationsGroup);
    
    // Add sample devices for demonstration
    QStandardItem* item1 = new QStandardItem("STM32H573I-DK #1");
    QStandardItem* type1 = new QStandardItem("STM32H5 Series");
    QStandardItem* status1 = new QStandardItem("Disconnected");
    QStandardItem* id1 = new QStandardItem("STM32H573I-001");
    m_device.deviceModel->appendRow({item1, type1, status1, id1});
    
    QStandardItem* item2 = new QStandardItem("STM32H573I-DK #2");
    QStandardItem* type2 = new QStandardItem("STM32H5 Series");
    QStandardItem* status2 = new QStandardItem("Connected");
    QStandardItem* id2 = new QStandardItem("STM32H573I-002");
    m_device.deviceModel->appendRow({item2, type2, status2, id2});
    
    // Connect signals
    connect(m_device.refreshButton, &QPushButton::clicked, this, &MainWindow::showDeviceManagement);
    
    return deviceTab;
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
    
    // Connect batch add files button
    connect(m_batch.addButton, &QPushButton::clicked, [this]() {
        QStringList files = QFileDialog::getOpenFileNames(
            this, "Select Files to Process", QDir::homePath(),
            m_batch.operationCombo->currentIndex() == 0 ? ALL_FILES_FILTER : ENCRYPTED_FILES_FILTER
        );
        
        for (const QString& file : files) {
            QStandardItem* fileItem = new QStandardItem(file);
            
            QString outputPath = file;
            if (m_batch.operationCombo->currentIndex() == 0) { // Encrypt
                outputPath += ENCRYPTED_EXTENSION;
            } else { // Decrypt
                if (outputPath.endsWith(ENCRYPTED_EXTENSION)) {
                    outputPath.chop(strlen(ENCRYPTED_EXTENSION));
                } else {
                    outputPath += DECRYPTED_EXTENSION;
                }
            }
            
            QStandardItem* outputItem = new QStandardItem(outputPath);
            QStandardItem* statusItem = new QStandardItem("Pending");
            
            m_batch.fileModel->appendRow({fileItem, outputItem, statusItem});
        }
        
        updateUiState();
    });
    
    // Connect batch remove button
    connect(m_batch.removeButton, &QPushButton::clicked, [this]() {
        QModelIndexList selection = m_batch.fileTable->selectionModel()->selectedRows();
        if (!selection.isEmpty()) {
            m_batch.fileModel->removeRow(selection.first().row());
        }
        
        updateUiState();
    });
}

void MainWindow::updateUiState()
{
    // Update encrypt button state
    bool encryptEnabled = !m_encrypt.fileEdit->text().isEmpty() &&
                         !m_encrypt.outputEdit->text().isEmpty() &&
                         !m_encrypt.passwordEdit->text().isEmpty();
    m_encrypt.button->setEnabled(encryptEnabled);
    
    // Update decrypt button state
    bool decryptEnabled = !m_decrypt.fileEdit->text().isEmpty() &&
                         !m_decrypt.outputEdit->text().isEmpty() &&
                         !m_decrypt.passwordEdit->text().isEmpty();
    m_decrypt.button->setEnabled(decryptEnabled);
    
    // Update batch button state
    bool batchEnabled = m_batch.fileModel->rowCount() > 0 &&
                       !m_batch.passwordEdit->text().isEmpty();
    m_batch.button->setEnabled(batchEnabled);
    
    // Update 2FA config button state
    m_encrypt.twoFactorConfigButton->setEnabled(m_encrypt.twoFactorCheckBox->isChecked());
}

void MainWindow::encryptFile()
{
    const QString& sourcePath = m_encrypt.fileEdit->text();
    const QString& destPath = m_encrypt.outputEdit->text();
    const QString& password = m_encrypt.passwordEdit->text();
    
    if (!confirmFileOverwrite(destPath)) {
        return;
    }
    
    processCryptoOperation(
        [this](const std::string& src, const std::string& dst, const std::string& pwd, 
               const std::string& secondFactor, const std::function<void(float)>& progressCallback) {
            // Note: secondFactor is ignored as the current implementation doesn't support it
            m_encryptor.encryptFile(src, dst, pwd, progressCallback);
        },
        sourcePath,
        destPath,
        password,
        m_encrypt.twoFactorCheckBox->isChecked() ? "2fa-token" : "",
        "File encrypted successfully"
    );
}

void MainWindow::decryptFile()
{
    const QString& sourcePath = m_decrypt.fileEdit->text();
    const QString& destPath = m_decrypt.outputEdit->text();
    const QString& password = m_decrypt.passwordEdit->text();
    const QString& secondFactor = m_decrypt.secondFactorEdit->text();
    
    if (!confirmFileOverwrite(destPath)) {
        return;
    }
    
    processCryptoOperation(
        [this](const std::string& src, const std::string& dst, const std::string& pwd, 
               const std::string& secondFactor, const std::function<void(float)>& progressCallback) {
            // Note: secondFactor is ignored as the current implementation doesn't support it
            m_encryptor.decryptFile(src, dst, pwd, progressCallback);
        },
        sourcePath,
        destPath,
        password,
        secondFactor,
        "File decrypted successfully"
    );
}

void MainWindow::processBatch()
{
    // TODO: Implement batch processing
    showStatusMessage("Batch processing not yet implemented", true);
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Open File", QDir::homePath(), ALL_FILES_FILTER
    );
    
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile()) {
            if (fileInfo.suffix() == "encrypted") {
                m_decrypt.fileEdit->setText(filePath);
                m_operationTabWidget->setCurrentIndex(1); // Switch to decrypt tab
            } else {
                m_encrypt.fileEdit->setText(filePath);
                m_operationTabWidget->setCurrentIndex(0); // Switch to encrypt tab
            }
        }
    }
}

void MainWindow::refreshFileList()
{
    // TODO: Implement file list refresh
    showStatusMessage("File list refresh not yet implemented");
}

void MainWindow::onFileSelected(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }
    
    // Get the file path from the hidden column
    QString filePath = m_fileSortModel->data(
        m_fileSortModel->index(index.row(), FILE_PATH_COLUMN)
    ).toString();
    
    if (filePath.isEmpty()) {
        return;
    }
    
    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile()) {
        if (fileInfo.suffix() == "encrypted") {
            m_decrypt.fileEdit->setText(filePath);
            m_operationTabWidget->setCurrentIndex(1); // Switch to decrypt tab
        } else {
            m_encrypt.fileEdit->setText(filePath);
            m_operationTabWidget->setCurrentIndex(0); // Switch to encrypt tab
        }
    }
}

void MainWindow::showSettings()
{
    // TODO: Implement settings dialog
    showStatusMessage("Settings dialog not yet implemented");
}

void MainWindow::showKeyManagement()
{
    // TODO: Implement key management
    showStatusMessage("Key management not yet implemented");
}

void MainWindow::showDeviceManagement()
{
    m_operationTabWidget->setCurrentIndex(3); // Switch to device tab
}

void MainWindow::toggle2FA(bool enabled)
{
    m_encrypt.twoFactorConfigButton->setEnabled(enabled);
}

void MainWindow::configure2FA()
{
    // TODO: Implement 2FA configuration
    showStatusMessage("2FA configuration not yet implemented");
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About CRUSTy-Core",
        "<h3>CRUSTy-Core</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A secure file encryption utility using Rust cryptography.</p>"
        "<p>&copy; 2025 CRUSTy-Core Team</p>"
    );
}

void MainWindow::showStatusMessage(const QString& message, bool isError)
{
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(isError ? "color: red;" : "color: green;");
    m_statusTimer.start(STATUS_MESSAGE_DURATION_MS);
}

bool MainWindow::confirmFileOverwrite(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            "Confirm Overwrite",
            QString("The file '%1' already exists. Do you want to overwrite it?").arg(filePath),
            QMessageBox::Yes | QMessageBox::No
        );
        
        return result == QMessageBox::Yes;
    }
    
    return true;
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
    // Show progress bar
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    
    // Disable UI during operation
    setEnabled(false);
    
    // Create progress callback
    auto progressCallback = [this](float progress) {
        // Update progress bar from the main thread
        QMetaObject::invokeMethod(m_progressBar, "setValue", Qt::QueuedConnection,
            Q_ARG(int, static_cast<int>(progress * 100)));
    };
    
    // Run operation in a separate thread
    std::thread([=, this]() {
        try {
            operation(
                sourcePath.toStdString(),
                destPath.toStdString(),
                password.toStdString(),
                secondFactor.toStdString(),
                progressCallback
            );
            
            // Operation completed successfully
            QMetaObject::invokeMethod(this, [this, successMessage]() {
                showStatusMessage(successMessage);
                m_progressBar->setVisible(false);
                setEnabled(true);
            }, Qt::QueuedConnection);
        }
        catch (const std::exception& e) {
            // Operation failed
            QMetaObject::invokeMethod(this, [this, e]() {
                showStatusMessage(QString("Error: %1").arg(e.what()), true);
                m_progressBar->setVisible(false);
                setEnabled(true);
            }, Qt::QueuedConnection);
        }
    }).detach();
}

} // namespace crusty
