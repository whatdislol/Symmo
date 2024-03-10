// audiocontrol.cpp

#include "audiocontrol.h"

AudioControl::AudioControl(QObject* parent) 
    : QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_ambiencePlayer(new QMediaPlayer(this)),
    m_ambienceOutput(new QAudioOutput(this)),
    m_totalDuration(0)
{
    m_player->setAudioOutput(m_audioOutput);
    setVolume(50);
    m_ambiencePlayer->setAudioOutput(m_ambienceOutput);
    m_ambiencePlayer->setLoops(QMediaPlayer::Infinite);
    setAmbienceVolume(20);
    m_ambiencePaths = {
        "", 
        getProjectRootPath() + "/asset/ambience_sounds/rain.mp3",
        getProjectRootPath() + "/asset/ambience_sounds/campfire.mp3"
    };
}

AudioControl::~AudioControl()
{
    delete m_player;
    delete m_audioOutput;
    delete m_ambiencePlayer;
    delete m_ambienceOutput;
}

void AudioControl::setVolume(int volume)
{
    m_audioOutput->setVolume(volume / 100.0);
    if (volume == 0) {
        m_audioOutput->setMuted(false);
		m_ambienceOutput->setMuted(true);
		emit isZeroVolume(true);
	}
    else if (!m_audioOutput->isMuted()){
		m_ambienceOutput->setMuted(false);
		emit isZeroVolume(false);
	}   
}

void AudioControl::toggleMute()
{
    if (m_audioOutput->volume() != 0) {
        if (m_audioOutput->isMuted()) {
            m_audioOutput->setMuted(false);
			m_ambienceOutput->setMuted(false);
        }
        else {
            m_audioOutput->setMuted(true);
			m_ambienceOutput->setMuted(true);
        }
    }
}

void AudioControl::togglePlayPause()
{
    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        if (m_player->isPlaying()) {
            m_player->pause();
            m_ambiencePlayer->pause();
        }
        else {
            m_player->play();
            m_ambiencePlayer->play();
        }
    }
}

void AudioControl::setPosition(int position)
{
    m_player->setPosition(static_cast<qint64>(position) * 1000);
}

QMediaPlayer* AudioControl::getMediaPlayer() const
{
	return m_player;
}

QAudioOutput* AudioControl::getAudioOutput() const
{
    return m_audioOutput;
}

QMediaPlayer* AudioControl::getAmbiencePlayer() const
{
    return m_ambiencePlayer;
}

void AudioControl::setTotalDuration(qint64& duration)
{
	m_totalDuration = duration;
}

qint64 AudioControl::getTotalDuration() const
{
	return m_totalDuration;
}

void AudioControl::setAmbienceVolume(int volume)
{
	m_ambienceOutput->setVolume(volume / 200.0);
}

void AudioControl::playAmbience(int index)
{
    m_ambiencePlayer->setSource(QUrl::fromLocalFile(m_ambiencePaths[index]));
    if (m_player->isPlaying()) {
        m_ambiencePlayer->play();
    }
    emit gifUpdated();
}

QString AudioControl::getProjectRootPath() const
{
    QString executablePath = QCoreApplication::applicationDirPath();
    QDir currentDir(executablePath);
    while (!currentDir.exists("CMakeLists.txt") && currentDir.cdUp());

    return currentDir.absolutePath();
}
