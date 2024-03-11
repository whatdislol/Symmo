#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "filepath.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "symmo_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.setMaximumSize(1150, 625);
    w.setMinimumSize(1150, 625);
    w.setWindowTitle("Symmo");
    w.setWindowIcon(QIcon(FilePath::getProjectRootPath() + "/asset/icons/symmo.png"));
    w.show();

    return a.exec();
}