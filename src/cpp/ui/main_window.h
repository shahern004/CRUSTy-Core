#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
#include <QTreeView>
#include <QListView>
#include <QTabWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QTableView>
#include <QTimer>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "../core/encryptor.h"
#include "../core/file_operations.h"

namespace crusty {

// Forward declarations
class FilePathSelector;
class FileListModel;
class PasswordStrengthMeter;
class DeviceManager;

/**
 * @brief Main application window
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * 
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MainWindow() override = default;
    
private slots:
    /**
     * @brief Encrypt the selected file
     */
    void encryptFile();
    
    /**
     * @brief Decrypt the selected file
     */
    void decryptFile();
    
    /**
     * @brief Process multiple files in batch mode
     */
    void processBatch();
    
    /**
     * @brief Open a file or directory
     */
    void openFile();
    
    /**
     * @brief Refresh the file list
     * 
     * @param directory Optional directory to navigate to
     */
    void refreshFileList(const QString& directory = QString());
    
    /**
     * @brief Handle file selection in the file list
     * 
     * @param index The selected index
     */
    void onFileSelected(const QModelIndex& index);
    
    /**
     * @brief Handle double-click on a file or directory
     * 
     * @param index The double-clicked index
     */
    void onFileDoubleClicked(const QModelIndex& index);
    
    /**
     * @brief Navigate up one directory level
     */
    void navigateUp();
    
    /**
     * @brief Show the settings dialog
     */
    void showSettings();
    
    /**
     * @brief Show the key management dialog
     */
    void showKeyManagement();
    
    /**
     * @brief Show the device management panel
     */
    void showDeviceManagement();
    
    /**
     * @brief Show about dialog
     */
    void showAbout();
    
private:
    /**
     * @brief Set up the user interface
     */
    void setupUi();
    
    /**
     * @brief Apply the application style sheet
     */
    void applyStyleSheet();
    
    /**
     * @brief Create menus and actions
     */
    void setupMenus();
    
    /**
     * @brief Create the toolbar
     */
    void setupToolBar();
    
    /**
     * @brief Update UI elements enabled state based on current input values
     */
    void updateUiState();
    
    /**
     * @brief Create the file browser panel
     * @return The file browser widget
     */
    QWidget* createFileBrowserPanel();
    
    /**
     * @brief Create the encryption tab
     * @return The encryption tab widget
     */
    QWidget* createEncryptTab();
    
    /**
     * @brief Create the decryption tab
     * @return The decryption tab widget
     */
    QWidget* createDecryptTab();
    
    /**
     * @brief Create the batch processing tab
     * @return The batch processing tab widget
     */
    QWidget* createBatchTab();
    
    /**
     * @brief Create the device management tab
     * @return The device management tab widget
     */
    QWidget* createDeviceTab();
    
    /**
     * @brief Set up file selection dialogs with their callbacks
     */
    void setupFileSelectors();
    
    /**
     * @brief Show a status message
     * 
     * @param message Message to show
     * @param isError True if the message is an error
     */
    void showStatusMessage(const QString& message, bool isError = false);
    
    /**
     * @brief Process a cryptographic operation in a background thread
     * 
     * @param operation The operation to perform (encrypt/decrypt)
     * @param sourcePath Source file path
     * @param destPath Destination file path
     * @param password Password for encryption/decryption
     */
    void processCryptoOperation(
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
        const QString& successMessage = "Operation completed successfully"
    );
    
    /**
     * @brief Check if the file exists and confirm overwrite if needed
     * 
     * @param filePath Path to check
     * @return true if operation can proceed, false otherwise
     */
    bool confirmFileOverwrite(const QString& filePath);
    
    // UI elements - Main layout
    QSplitter* m_mainSplitter;
    QTreeView* m_fileTreeView;
    QTabWidget* m_operationTabWidget;
    QStandardItemModel* m_fileModel;
    QSortFilterProxyModel* m_fileSortModel;
    QString m_currentDirectory;  // Current directory being displayed
    QLineEdit* m_pathEdit;       // Address bar path edit
    
    // UI elements - Toolbar
    QToolBar* m_toolBar;
    QAction* m_openAction;
    QAction* m_encryptAction;
    QAction* m_decryptAction;
    QAction* m_batchAction;
    QAction* m_deviceAction;
    QAction* m_keyAction;
    QAction* m_settingsAction;
    
    // UI elements - Encrypt tab
    struct {
        QLineEdit* fileEdit;
        QLineEdit* outputEdit;
        QLineEdit* passwordEdit;
        PasswordStrengthMeter* strengthMeter;
        QPushButton* button;
    } m_encrypt;
    
    // UI elements - Decrypt tab
    struct {
        QLineEdit* fileEdit;
        QLineEdit* outputEdit;
        QLineEdit* passwordEdit;
        QPushButton* button;
    } m_decrypt;
    
    // UI elements - Batch tab
    struct {
        QTableView* fileTable;
        QStandardItemModel* fileModel;
        QPushButton* addButton;
        QPushButton* removeButton;
        QComboBox* operationCombo;
        QLineEdit* passwordEdit;
        QPushButton* button;
    } m_batch;
    
    // UI elements - Device tab
    struct {
        QTableView* deviceTable;
        QStandardItemModel* deviceModel;
        QPushButton* refreshButton;
        QPushButton* connectButton;
        QPushButton* installButton;
        QLabel* statusLabel;
    } m_device;
    
    // Status and progress
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QTimer m_statusTimer;
    
    // Core components
    Encryptor m_encryptor;
};

/**
 * @brief Password strength meter widget
 */
class PasswordStrengthMeter : public QProgressBar {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * 
     * @param parent Parent widget
     */
    explicit PasswordStrengthMeter(QWidget* parent = nullptr);
    
public slots:
    /**
     * @brief Update the strength meter based on password
     * 
     * @param password The password to evaluate
     */
    void updateStrength(const QString& password);
    
private:
    /**
     * @brief Calculate password strength
     * 
     * @param password The password to evaluate
     * @return int Strength score (0-100)
     */
    int calculateStrength(const QString& password);
};

} // namespace crusty
