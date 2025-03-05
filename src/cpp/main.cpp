#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#include "ui/main_window.h"
#include "core/encryptor.h"

int main(int argc, char *argv[]) {
    // Create Qt application
    QApplication app(argc, argv);
    
    // Set application information
    QApplication::setApplicationName("CRUSTy-Qt");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("CRUSTy");
    QApplication::setOrganizationDomain("crusty.example.com");
    
    try {
        // Create main window
        crusty::MainWindow mainWindow;
        mainWindow.show();
        
        // Run application
        return app.exec();
    } catch (const std::exception& e) {
        // Show error message
        QMessageBox::critical(
            nullptr,
            "Error",
            QString("An error occurred: %1").arg(e.what())
        );
        
        return 1;
    } catch (...) {
        // Show error message for unknown exceptions
        QMessageBox::critical(
            nullptr,
            "Error",
            "An unknown error occurred."
        );
        
        return 1;
    }
}
