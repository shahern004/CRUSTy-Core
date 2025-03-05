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
     * @brief Select a file for encryption
     */
    void selectEncryptFile();
    
    /**
     * @brief Select a file for decryption
     */
    void selectDecryptFile();
    
    /**
     * @brief Select output directory for encryption
     */
    void selectEncryptOutput();
    
    /**
     * @brief Select output directory for decryption
     */
    void selectDecryptOutput();
    
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
     * @brief Create actions
     */
    void createActions();
    
    /**
     * @brief Create menus
     */
    void createMenus();
    
    /**
     * @brief Update the UI state
     */
    void updateUiState();
    
    /**
     * @brief Show a status message
     * 
     * @param message Message to show
     * @param isError True if the message is an error
     */
    void showStatusMessage(const QString& message, bool isError = false);
    
    // UI elements
    QLabel *m_encryptFileLabel;
    QLineEdit *m_encryptFileEdit;
    QPushButton *m_encryptFileBrowseButton;
    
    QLabel *m_encryptOutputLabel;
    QLineEdit *m_encryptOutputEdit;
    QPushButton *m_encryptOutputBrowseButton;
    
    QLabel *m_encryptPasswordLabel;
    QLineEdit *m_encryptPasswordEdit;
    
    QPushButton *m_encryptButton;
    
    QLabel *m_decryptFileLabel;
    QLineEdit *m_decryptFileEdit;
    QPushButton *m_decryptFileBrowseButton;
    
    QLabel *m_decryptOutputLabel;
    QLineEdit *m_decryptOutputEdit;
    QPushButton *m_decryptOutputBrowseButton;
    
    QLabel *m_decryptPasswordLabel;
    QLineEdit *m_decryptPasswordEdit;
    
    QLabel *m_decryptSecondFactorLabel;
    QLineEdit *m_decryptSecondFactorEdit;
    
    QPushButton *m_decryptButton;
    
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    // Core components
    Encryptor m_encryptor;
    
    // Status message timer
    QTimer m_statusTimer;
};

} // namespace crusty
