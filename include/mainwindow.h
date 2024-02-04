#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtWidgets>
#include <QtMultimedia>
#include <QFileDialog>
#include <QStyle>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Volumn_clicked();

    void on_pushButton_Play_clicked();

    void on_actionAdd_Fiel_triggered();

    void on_horizontalSlider_song_duration_valueChanged(int value);

private:
    Ui::MainWindow* ui;
    bool is_muted = false;
    bool is_pause = false;
    QMediaPlayer* M_Player;
};
#endif // MAINWINDOW_H