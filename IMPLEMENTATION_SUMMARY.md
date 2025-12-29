# Region Selection and Drawing Implementation

## Overview
This implementation adds region selection, automatic zoom, and drawing capabilities to the ImageProssersor application as per the requirements.

## Features Implemented

### 1. Region Selection on Main Window
- **Click and Drag**: Users can click and drag on the main image to select a rectangular region
- **Visual Feedback**: The selected region is highlighted with a red dashed rectangle during selection
- **Minimum Size**: A minimum selection size of 5x5 pixels is enforced to distinguish from simple clicks

### 2. Automatic Geometry Transform Navigation
- **Auto-Open**: After completing a valid region selection (> 5x5 pixels), the geometry transform window automatically opens
- **Region Display**: Only the selected region is passed to the geometry transform window
- **Zoom Effect**: 
  - Smaller selections appear magnified (zoomed in) in the transform window
  - Larger selections appear closer to the original size
  - The transform window size remains constant, providing automatic zoom based on selection size

### 3. Drawing Capability on Transform Window
- **Free Drawing**: Users can draw on the image in the geometry transform window using the mouse
- **Drawing Style**: Red pen with 3-pixel width, rounded caps and joins
- **Persistent Drawing**: Drawings persist through transform operations
- **Left Click to Draw**: Hold left mouse button and drag to draw

## Technical Implementation

### Changes to ImageProssersor Class
- Added member variables for selection tracking: `isSelecting`, `selectionStart`, `selectionEnd`, `selectionRect`
- Enhanced `eventFilter()` to handle:
  - MouseButtonPress: Start selection
  - MouseMove: Update selection rectangle with visual feedback
  - MouseButtonRelease: Complete selection and trigger geometry transform
- Modified `showGeometryTransform()` to:
  - Extract selected region from the original image
  - Convert widget coordinates to image coordinates
  - Pass cropped image to ImageTransform window

### Changes to ImageTransform Class
- Added `displayPixmap` as public member to store the displayed pixmap including drawings
- Implemented `eventFilter()` to handle drawing:
  - MouseButtonPress: Start drawing, initialize displayPixmap
  - MouseMove: Draw lines on displayPixmap while mouse button is held
  - MouseButtonRelease: Stop drawing
- Updated all transform operations (mirror, rotate) to use displayPixmap
- Drawing is done directly on the pixmap using QPainter

## User Workflow

1. **Load an Image**: Open an image file through File > Open
2. **Select Region**: Click and drag on the image to select a region of interest
3. **Auto-Navigate**: Release mouse button to automatically open the geometry transform window
4. **View Zoomed Region**: The selected region appears in the transform window, automatically scaled
5. **Draw on Image**: Click and drag in the transform window to draw on the image
6. **Apply Transforms**: Use the existing mirror and rotate controls
7. **Save Result**: Save the modified image using the Save button

## Build Requirements
- Qt 6.x (tested with Qt 6.4.2)
- C++17 compiler
- qmake6 build system

## Building
```bash
qmake6 ImageProssersor.pro
make
```

## Notes
- The geometry transform window maintains a constant size for consistent UX
- Drawing capability is independent of the source of the image (whether from region selection or full image)
- All existing functionality (mirror, rotate, open, save) continues to work as before
- The .gitignore file has been created to exclude build artifacts
