function myS() {
  document.getElementById("demo").innerHTML = "Paragraph changed.";
}
function myB() {
  document.getElementById("demo").innerHTML = "Hello world";
}

function parameterFuntion(a, b) {
  let result = a + b
  let output = document.getElementById("parameter").innerHTML = result
  return output;
}

function ifElse(number) {
  if (number < 10) {
    return document.getElementById("parameter").innerHTML = "result < 10"
  } else if (number < 20) {
    return document.getElementById("parameter").innerHTML = "result < 20"
  } else {
    return document.getElementById("parameter").innerHTML = "result > 20"
  }
}

function switchCase(x) {
  switch (x) {
    case 0:
      return document.getElementById("parameter").innerHTML = "off"
    case 1:
      return document.getElementById("parameter").innerHTML = "on"
    default:
      return document.getElementById("parameter").innerHTML = "valid value"
  }
}

function forLoop() {
  let text = "";
  for (let i = 0; i < 5; i++) {
    text += "The number is " + i + "<br>";
  }
  return document.getElementById("demo").innerHTML = text;
}

function forLoopObject() {
  const cars = ["BMW", "Volvo", "Saab", "Ford"];
  let i, len, text;
  for (i = 0, len = cars.length, text = ""; i < len; i++) {
    text += cars[i] + "<br>";
  }
  return document.getElementById("demo").innerHTML = text;
}