const addon = require('./build/Release/addon');

const visibleWindows = addon.getVisibleWindows();
console.log(visibleWindows);

const spotifyWindow = visibleWindows.find((window) => window.processName === 'Spotify.exe');

if (!spotifyWindow) {
  throw new Error('Spotify is not running');
}

addon.startLoopbackCapture(spotifyWindow.processId, (data) => console.log(data));
