// audiocontrol.cpp

#include "audiocontrol.h"

AudioControl::AudioControl(QObject* parent) 
    : QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_totalDuration(0)
{
    m_player->setAudioOutput(m_audioOutput);
    setVolume(50);
}

AudioControl::~AudioControl()
{
    delete m_player;
    delete m_audioOutput;
}

void AudioControl::setVolume(int volume)
{
    m_audioOutput->setVolume(volume / 100.0);
}

void AudioControl::toggleMute()
{
    if (isMuted()) {
        m_audioOutput->setMuted(false);
    }
    else {
        m_audioOutput->setMuted(true);
    }
    emit muteStateChanged();
}

void AudioControl::togglePlayPause()
{
    if (isPlaying()) {
        m_player->pause();
    }
    else {
        m_player->play();
    }
    emit playPauseStateChanged();
}

bool AudioControl::isMuted() const
{
    return m_audioOutput->isMuted();
}

bool AudioControl::isPlaying() const
{
    return m_player->isPlaying();
}

void AudioControl::setPosition(int position)
{
    m_player->setPosition(static_cast<qint64>(position) * 1000);
}

QMediaPlayer* AudioControl::getMediaPlayer() const
{
	return m_player;
}

void AudioControl::setTotalDuration(qint64& duration)
{
	m_totalDuration = duration;
}

qint64 AudioControl::getTotalDuration() const
{
	return m_totalDuration;
}