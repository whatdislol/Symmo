// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QStyle>
#include <QSlider>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "audiocontrol.h"
#include "playlistmanager.h"

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

signals:
    void playlistAdded(QString playlistName);

private slots:
    void setMaxDuration(qint64 duration);
    void updateSongProgress(qint64 progress);
    void updateDuration(qint64 duration);
    void updatePlaybackUI(QMediaPlayer::MediaStatus status);
    void updateMuteIcon(bool muted);
    void updatePlayPauseIcon(bool playing);
    void updateShuffleIcon(bool shuffled);
    void on_actionAdd_File_triggered();
    void setupIcons();
    void addSongWidgetItem(QListWidgetItem* song);
    void addPlaylistWidgetItem(QListWidgetItem* playlist);
    void getNewPlaylistName();
    void updateSongsDisplay();
    void updatePlaylistInfo();
    void updatePlaylistDisplay();
    void removePlaylist(const int& index);
    void showContextMenu(const QPoint& pos);
    void saveToJSON(const QString& filePath);
    void loadFromJSON(const QString& filePath);

private:
    Ui::MainWindow* ui;
    AudioControl* m_audioControl;
    PlaylistManager* m_playlistManager;
    QString getProjectRootPath() const;
    QString m_dataPath;
};

#endif // MAINWINDOW_H