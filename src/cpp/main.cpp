#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#include "ui/main_window.h"
#include "core/encryptor.h"

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

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        configureApplication(app);
        
        crusty::MainWindow mainWindow;
        mainWindow.show();
        
        return app.exec();
    } catch (const std::exception& e) {
        showErrorMessage(QString("An error occurred: %1").arg(e.what()));
        return 1;
    } catch (...) {
        showErrorMessage("An unknown error occurred.");
        return 1;
    }
}
