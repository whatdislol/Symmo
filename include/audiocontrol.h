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
    void setPosition(int position);
    QMediaPlayer* getMediaPlayer() const;
    void setTotalDuration(qint64& duration);
    qint64 getTotalDuration() const;

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void muteStateChanged(bool m_isMuted);
    void playPauseStateChanged(bool m_isPaused);

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    bool m_isMuted;
    bool m_isPaused;
    qint64 m_totalDuration;
};

#endif // AUDIOCONTROL_H
