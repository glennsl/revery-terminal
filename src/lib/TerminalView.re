module Color = Revery.Color;
module Colors = Revery.Colors;
open Revery.Draw;
open Revery.UI;

let make =
    (
      ~theme=Theme.default,
      ~screen: Screen.t,
      ~cursor: Cursor.t,
      ~font: Font.t,
      (),
    ) => {
  let containerStyle =
    Style.[
      backgroundColor(Colors.black),
      position(`Absolute),
      justifyContent(`Center),
      alignItems(`Center),
      bottom(0),
      top(0),
      left(0),
      right(0),
    ];

  let getColor = (color: Vterm.Color.t) => {
    switch (color) {
    | DefaultBackground => theme(0)
    | DefaultForeground => theme(15)
    | Rgb(r, g, b) =>
      Skia.Color.makeArgb(
        255l,
        r |> Int32.of_int,
        g |> Int32.of_int,
        b |> Int32.of_int,
      )
    | Index(idx) => theme(idx)
    };
  };

  let getFgColor = (cell: Vterm.ScreenCell.t) => {
    cell.reverse == 0 ? getColor(cell.fg) : getColor(cell.bg);
  };

  let getBgColor = (cell: Vterm.ScreenCell.t) => {
    cell.reverse == 0 ? getColor(cell.bg) : getColor(cell.fg);
  };

  let element =
    <Canvas
      style=containerStyle
      render={canvasContext => {
        let {font, lineHeight, characterWidth, characterHeight, fontSize}: Font.t = font;
        let defaultBackground = Colors.black |> Color.toSkia;

        let backgroundPaint = Skia.Paint.make();
        Skia.Paint.setAntiAlias(backgroundPaint, false);

        let textPaint = Skia.Paint.make();
        let typeFace = Revery.Font.getSkiaTypeface(font);
        Skia.Paint.setTypeface(textPaint, typeFace);
        Skia.Paint.setTextSize(textPaint, fontSize);
        Skia.Paint.setAntiAlias(textPaint, true);
        Skia.Paint.setSubpixelText(textPaint, true);
        Skia.Paint.setLcdRenderText(textPaint, true);

        let columns = Screen.getColumns(screen);
        let rows = Screen.getRows(screen);
        for (column in 0 to columns - 1) {
          for (row in 0 to rows - 1) {
            let cell = Screen.getCell(~row, ~column, screen);

            let bgColor = getBgColor(cell);
            if (bgColor != defaultBackground) {
              Skia.Paint.setColor(backgroundPaint, bgColor);
              CanvasContext.drawRectLtwh(
                ~paint=backgroundPaint,
                ~left=float(column) *. characterWidth,
                ~top=float(row) *. lineHeight,
                ~height=lineHeight,
                ~width=characterWidth,
                canvasContext,
              );
            };
          };
        };

        for (column in 0 to columns - 1) {
          for (row in 0 to rows - 1) {
            let cell = Screen.getCell(~row, ~column, screen);

            let fgColor = getFgColor(cell);

            Skia.Paint.setColor(textPaint, fgColor);
            if (String.length(cell.chars) > 0) {
              let char = cell.chars.[0];
              let code = Char.code(char);
              if (code != 0) {
                CanvasContext.drawText(
                  ~paint=textPaint,
                  ~x=float(column) *. characterWidth,
                  ~y=float(row) *. lineHeight +. characterHeight,
                  ~text=String.make(1, cell.chars.[0]),
                  canvasContext,
                );
              };
            };
          };
        };

        // If the cursor is visible, let's paint it now
        if (cursor.visible) {
          Skia.Paint.setColor(textPaint, Colors.white |> Color.toSkia);
          CanvasContext.drawRectLtwh(
            ~paint=textPaint,
            ~left=float(cursor.column) *. characterWidth,
            ~top=float(cursor.row) *. lineHeight,
            ~width=characterWidth,
            ~height=lineHeight,
            canvasContext,
          );
        };
      }}
    />;

  element;
};
