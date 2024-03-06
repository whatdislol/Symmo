// audiocontrol.h

#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QCoreApplication>
#include <QDir>

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
    QAudioOutput* getAudioOutput() const;
    void setTotalDuration(qint64& duration);
    qint64 getTotalDuration() const;
    void setAmbienceVolume(int volume);
    void playAmbience(int index);

signals:
    void isZeroVolume(bool muted);

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QMediaPlayer* m_ambiencePlayer;
    QAudioOutput* m_ambienceOutput;
    QStringList m_ambiencePaths;
    qint64 m_totalDuration;
    QString getProjectRootPath() const;
};

#endif // AUDIOCONTROL_H
