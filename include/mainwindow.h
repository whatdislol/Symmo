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
#include <QMenu>
#include <QMovie>
#include <QFontDataBase>
#include <QFont>
#include <QTimer>
#include <QFontMetrics>
#include "audiocontrol.h"
#include "playlistmanager.h"
#include "filepath.h"

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
    void updateNextSongName();
    void updateMuteIcon(bool muted);
    void updatePlayPauseIcon(bool playing);
    void updateShuffleIcon(bool shuffled);
    void updateLoopIcon(bool looped);
    void setupUI();
    void setupFonts() const;
    void addSongWidgetItem(QListWidgetItem* song);
    void addPlaylistWidgetItem(QListWidgetItem* playlist);
    void getNewPlaylistName();
    void updatePlaylistsDisplay(const int& index, const QString& name);
    void updateSongsDisplay();
    void updatePlaylistInfo();
    void updateOnPlaylistSelected();
    void changeGif();
    void updateGifState(QMediaPlayer::PlaybackState state);
    void removePlaylist(const int& index);
    void scrollOverflownText();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onSongProgressSliderReleased();
    void onShuffleStatusChanged(bool shuffled);
    void filterSearchResults(const QString& searchQuery);
    void setSongProgressValue();
    void showContextMenu(const QPoint& pos);
    void saveToJSON(const QString& filePath);
    void loadFromJSON(const QString& filePath);

private:
    Ui::MainWindow* ui;
    AudioControl* m_audioControl;
    PlaylistManager* m_playlistManager;
    QString m_dataPath;
    QString m_assetPath;
    QMovie* m_gif;
    QTimer* m_timer;
    bool m_appStartup;
};

#endif // MAINWINDOW_H