// audiocontrol.cpp

#include "audiocontrol.h"

AudioControl::AudioControl(QObject* parent) 
    : QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_isMuted(false),
    m_isPaused(true),
    m_totalDuration(0)
{
    m_player->setAudioOutput(m_audioOutput);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioControl::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioControl::positionChanged);
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
    if (m_isMuted) {
        m_audioOutput->setMuted(false);
    }
    else {
        m_audioOutput->setMuted(true);
    }
    m_isMuted = !m_isMuted;
    emit muteStateChanged(m_isMuted);
}

void AudioControl::togglePlayPause()
{
    if (m_isPaused) {
        m_player->play();
    }
    else {
        m_player->pause();
    }
    m_isPaused = !m_isPaused;
    emit playPauseStateChanged(m_isPaused);
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