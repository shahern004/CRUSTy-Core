#ifndef NO_QT_UI
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include "ui/main_window.h"
#else
#include <iostream>
#include <string>
#endif

#include "core/encryptor.h"

#ifndef NO_QT_UI
namespace {
    void configureApplication(QApplication& app) {
        app.setApplicationName("CRUSTy-Qt");
        app.setApplicationVersion("1.0.0");
        app.setOrganizationName("CRUSTy");
        app.setOrganizationDomain("crusty.example.com");
    }
    
    void showErrorMessage(const QString& message) {
        QMessageBox::critical(nullptr, "Error", message);
    }
}
#endif

int main(int argc, char *argv[]) {
    try {
#ifndef NO_QT_UI
        // Qt GUI version
        QApplication app(argc, argv);
        configureApplication(app);
        
        crusty::MainWindow mainWindow;
        mainWindow.show();
        
        return app.exec();
#else
        // CLI version
        std::cout << "CRUSTy-CLI v1.0.0" << std::endl;
        std::cout << "This is a command-line version built without Qt UI." << std::endl;
        
        // Basic functionality demo
        std::cout << "Initializing encryption engine..." << std::endl;
        // Add CLI functionality here
        
        return 0;
#endif
    } catch (const std::exception& e) {
#ifndef NO_QT_UI
        showErrorMessage(QString("An error occurred: %1").arg(e.what()));
#else
        std::cerr << "Error: " << e.what() << std::endl;
#endif
        return 1;
    } catch (...) {
#ifndef NO_QT_UI
        showErrorMessage("An unknown error occurred.");
#else
        std::cerr << "An unknown error occurred." << std::endl;
#endif
        return 1;
    }
}
