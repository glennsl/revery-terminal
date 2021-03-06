type t = {
  font: Revery.Font.t,
  fontSize: float,
  lineHeight: float,
  characterHeight: float,
  characterWidth: float,
};

let make = (~size, font: Revery.Font.t) => {
  let fontSize = size;
  let {height, lineHeight, _}: Revery.Font.FontMetrics.t =
    Revery.Font.getMetrics(font, fontSize);
  let {width, _}: Revery.Font.measureResult =
    Revery.Font.measure(font, fontSize, "M");

  {
    font,
    fontSize,
    lineHeight,
    characterHeight: height,
    characterWidth: width,
  };
};
