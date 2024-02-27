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
    bool isMuted() const;
    bool isPlaying() const;
    void setPosition(int position);
    QMediaPlayer* getMediaPlayer() const;
    void setTotalDuration(qint64& duration);
    qint64 getTotalDuration() const;

signals:
    void muteStateChanged();
    void playPauseStateChanged();

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    qint64 m_totalDuration;
};

#endif // AUDIOCONTROL_H
