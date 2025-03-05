#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>

#include "../core/encryptor.h"
#include "../core/file_operations.h"

namespace crusty {

// Forward declarations
class FilePathSelector;

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
     * @brief Show about dialog
     */
    void showAbout();
    
private:
    /**
     * @brief Set up the user interface
     */
    void setupUi();
    
    /**
     * @brief Create menus and actions
     */
    void setupMenus();
    
    /**
     * @brief Update UI elements enabled state based on current input values
     */
    void updateUiState();
    
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
     * @param secondFactor Optional second factor for decryption
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
        const QString& secondFactor = "",
        const QString& successMessage = "Operation completed successfully"
    );
    
    /**
     * @brief Check if the file exists and confirm overwrite if needed
     * 
     * @param filePath Path to check
     * @return true if operation can proceed, false otherwise
     */
    bool confirmFileOverwrite(const QString& filePath);
    
    // UI elements
    struct {
        QLineEdit* fileEdit;
        QLineEdit* outputEdit;
        QLineEdit* passwordEdit;
        QPushButton* button;
    } m_encrypt;
    
    struct {
        QLineEdit* fileEdit;
        QLineEdit* outputEdit;
        QLineEdit* passwordEdit;
        QLineEdit* secondFactorEdit;
        QPushButton* button;
    } m_decrypt;
    
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QTimer m_statusTimer;
    
    // Core components
    Encryptor m_encryptor;
};

} // namespace crusty
