// audiocontrol.cpp

#include "audiocontrol.h"

AudioControl::AudioControl(QObject* parent) : QObject(parent),
m_player(new QMediaPlayer(this)),
m_audioOutput(new QAudioOutput(this)),
m_isMuted(false),
m_isPaused(true)
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
    m_isMuted = !m_isMuted;
    m_audioOutput->setMuted(m_isMuted);
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
}

void AudioControl::loadMedia(const QUrl& url)
{
    m_player->setSource(url);
}

qint64 AudioControl::duration() const
{
    return m_player->duration();
}

qint64 AudioControl::position() const
{
    return m_player->position();
}

void AudioControl::setPosition(int position)
{
    m_player->setPosition(static_cast<qint64>(position) * 1000);
}

bool AudioControl::isMuted() const
{
	return m_isMuted;
}

bool AudioControl::isPaused() const
{
	return m_isPaused;
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