// audiocontrol.cpp

#include "audiocontrol.h"

AudioControl::AudioControl(QObject* parent) 
    : QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_ambiencePlayer(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_ambienceOutput(new QAudioOutput(this)),
    m_totalDuration(0)
{
    m_player->setAudioOutput(m_audioOutput);
    m_ambiencePlayer->setAudioOutput(m_ambienceOutput);
    setVolume(50);
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
		emit isZeroVolume(true);
	}
    else if (!m_audioOutput->isMuted()){
		emit isZeroVolume(false);
	}   
}

void AudioControl::toggleMute()
{
    if (m_audioOutput->volume() != 0) {
        if (m_audioOutput->isMuted()) {
            m_audioOutput->setMuted(false);
        }
        else {
            m_audioOutput->setMuted(true);
        }
    }
}

void AudioControl::togglePlayPause()
{
    if (m_player->isPlaying()) {
        m_player->pause();
    }
    else {
        m_player->play();
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
	m_ambienceOutput->setVolume(volume / 100.0);
}

void AudioControl::playAmbience(int index)
{
    if (index == -1) {
		m_ambiencePlayer->stop();
		return;
	}

	m_ambiencePlayer->setSource(QUrl::fromLocalFile(m_ambiencePaths[index]));
	m_ambiencePlayer->play();
}

QString AudioControl::getProjectRootPath() const
{
    QString executablePath = QCoreApplication::applicationDirPath();
    QDir currentDir(executablePath);
    while (!currentDir.exists("CMakeLists.txt") && currentDir.cdUp());

    return currentDir.absolutePath();
}
