// audiocontrol.h

#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class AudioControl : public QObject
{
    Q_OBJECT
public:
    explicit AudioControl(QObject* parent = nullptr);
    ~AudioControl();

    void setVolume(int volume);
    void toggleMute();
    void togglePlayPause();
    void loadMedia(const QUrl& url);
    qint64 duration() const;
    qint64 position() const;
    void setPosition(int position);
    bool isMuted() const;
    bool isPaused() const;
    QMediaPlayer* getMediaPlayer() const;

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    bool m_isMuted;
    bool m_isPaused;
};

#endif // AUDIOCONTROL_H
