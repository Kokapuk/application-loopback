# application-loopback

Capture window specific audio by process id.

> [!IMPORTANT]
> This package relies on Windows-specific APIs (EnumWindows, GetIconInfo, WASAPI loopback capture). Tested on Windows 11. Older versions of Windows may or may not work – no guarantees

## Basic usage

```js
const { getVisibleWindows, startLoopbackCapture, stopLoopbackCapture } = require('application-loopback');

const visibleWindows = getVisibleWindows();
const spotifyWindow = visibleWindows.find((window) => window.processName === 'Spotify.exe');

if (!spotifyWindow) {
  throw new Error('Spotify is not running');
}

startLoopbackCapture(
  spotifyWindow.processId,
  (data) => console.log(data),
  () => console.log('finished')
);

setTimeout(() => stopLoopbackCapture(), 3000);
```

## API Reference

### getVisibleWindows

Returns list of visible windows.

```ts
interface Window {
  processId: number;
  processName: string;
  title: string;
  icon: {
    buffer: Uint8Array;
    width: number;
    height: number;
  } | null;
}

getVisibleWindows(): Window[];
```

> [!TIP]
> To turn icon into an image you might use some external tools like `nativeImage` in electron.

```js
nativeImage
  .createFromBitmap(window.icon.buffer, {
    width: window.icon.width,
    height: window.icon.height,
  })
  .toDataURL();
```

### startLoopbackCapture

Starts loopback capture for window found by specified process id.

```ts
startLoopbackCapture(processId: number, chunkCallback: (chunk: Uint8Array) => void, finishCallback?: () => void): void;
```

> [!NOTE]
> `chunk` is raw PCM audio buffer

> [!WARNING]
> You can only capture one window at a time, call `stopLoopbackCapture` before calling `startLoopbackCapture` again

### stopLoopbackCapture

Stops active loopback capture if one exists

```ts
stopLoopbackCapture(): void;
```

## Remark

Addon is based on [original package](https://github.com/WerdoxDev/application-loopback) by WerdoxDev. Original package was really useful at least because most of C++ code i copied from it. But it had some encoding problems and i also wanted to include icons for windows, so i also decided to turn it into a native addon for better performance and developer experience. Maybe later i will also add typescript bindings for it, dunno for sure 🫤
