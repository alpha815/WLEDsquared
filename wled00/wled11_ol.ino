/*
 * Used to draw clock overlays over the strip
 */

void handleOverlays()
{
  if (millis() - overlayRefreshedTime > overlayRefreshMs)
  {
    updateLocalTime();
    checkTimers();

    if (overlayCurrent) _overlayAnalogClock();

    if (!countdownMode || !overlayCurrent) checkCountdown(); //countdown macro activation must work
    overlayRefreshedTime = millis();
  }
}

void _overlayAnalogClock()
{
  int overlaySize = overlayMax - overlayMin +1;
  strip.unlockAll();
  if (countdownMode)
  {
    _overlayAnalogCountdown(); return;
  }
  double hourP = ((double)(hour(local)%12))/12;
  double minuteP = ((double)minute(local))/60;
  hourP = hourP + minuteP/12;
  double secondP = ((double)second(local))/60;
  int hourPixel = floor(analogClock12pixel + overlaySize*hourP);
  if (hourPixel > overlayMax) hourPixel = overlayMin -1 + hourPixel - overlayMax;
  int minutePixel = floor(analogClock12pixel + overlaySize*minuteP);
  if (minutePixel > overlayMax) minutePixel = overlayMin -1 + minutePixel - overlayMax; 
  int secondPixel = floor(analogClock12pixel + overlaySize*secondP);
  if (secondPixel > overlayMax) secondPixel = overlayMin -1 + secondPixel - overlayMax;
  if (analogClockSecondsTrail)
  {
    if (secondPixel < analogClock12pixel)
    {
      strip.setRange(analogClock12pixel, overlayMax, 0xFF0000);
      strip.setRange(overlayMin, secondPixel, 0xFF0000);
    } else
    {
      strip.setRange(analogClock12pixel, secondPixel, 0xFF0000);
    }
  }
  if (analogClock5MinuteMarks)
  {
    int pix;
    for (int i = 0; i <= 12; i++)
    {
      pix = analogClock12pixel + round((overlaySize / 12.0) *i);
      if (pix > overlayMax) pix -= overlaySize;
      strip.setIndividual(pix, 0x00FFAA);
    }
  }
  if (!analogClockSecondsTrail) strip.setIndividual(secondPixel, 0xFF0000);
  strip.setIndividual(minutePixel, 0x00FF00);
  strip.setIndividual(hourPixel, 0x0000FF);
  overlayRefreshMs = 998;
}

void _overlayAnalogCountdown()
{
  strip.unlockAll();
  if (now() >= countdownTime)
  {
    checkCountdown();
  } else
  {
    long diff = countdownTime - now();
    double pval = 60;
    if (diff > 31557600L) //display in years if more than 365 days
    {
      pval = 315576000L; //10 years
    } else if (diff > 2592000L) //display in months if more than a month
    {
      pval = 31557600L; //1 year
    } else if (diff > 604800) //display in weeks if more than a week
    {
      pval = 2592000L; //1 month
    } else if (diff > 86400) //display in days if more than 24 hours
    {
      pval = 604800; //1 week
    } else if (diff > 3600) //display in hours if more than 60 minutes
    {
      pval = 86400; //1 day
    } else if (diff > 60) //display in minutes if more than 60 seconds
    {
      pval = 3600; //1 hour
    }
    int overlaySize = overlayMax - overlayMin +1;
    double perc = (pval-(double)diff)/pval;
    if (perc > 1.0) perc = 1.0;
    byte pixelCnt = perc*overlaySize;
    if (analogClock12pixel + pixelCnt > overlayMax)
    {
      strip.setRange(analogClock12pixel, overlayMax, ((uint32_t)whiteSec << 24)| ((uint32_t)colSec[0] << 16) | ((uint32_t)colSec[1] << 8) | colSec[2]);
      strip.setRange(overlayMin, overlayMin +pixelCnt -(1+ overlayMax -analogClock12pixel), ((uint32_t)whiteSec << 24)| ((uint32_t)colSec[0] << 16) | ((uint32_t)colSec[1] << 8) | colSec[2]);
    } else
    {
      strip.setRange(analogClock12pixel, analogClock12pixel + pixelCnt, ((uint32_t)whiteSec << 24)| ((uint32_t)colSec[0] << 16) | ((uint32_t)colSec[1] << 8) | colSec[2]);
    }
  }
  overlayRefreshMs = 998;
}
