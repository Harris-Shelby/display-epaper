const fs = require("fs");
const floydSteinberg = require("floyd-steinberg");
const sharp = require("sharp");
const PNG = require("pngjs").PNG;
const getPixels = require("get-pixels");
const { stringify } = require("querystring");

formatImgData(
  "/Users/weilaibushimeng/Documents/PlatformIO/Projects/display-epaper/server/cdd.png"
);

async function formatImgData(originImageUrl) {
  try {
    await sharp(originImageUrl)
      .resize({
        width: 128,
        height: 296,
      })
      .toFormat("jpeg")
      .png({ quality: 100 })
      .toFile(`./sharpImg.png`);

    // let OriginImgData = fs.readFileSync("./sharpImg.png");
    // let pngImgData = PNG.sync.read(OriginImgData);
    // let ditherImage = floydSteinberg(pngImgData);
    // let buffer = PNG.sync.write(ditherImage);

    // fs.writeFileSync("./ditherImg.png", buffer);

    let pixelImgData = await getPixelsPro("./sharpImg.png");
    let RGBAImgData = await formatRGBAs(pixelImgData);
    let b = [];
    let c = "";
    RGBAImgData.forEach((e, i) => {
      if (`${e.join("")}` === "1111") {
        b.push(1);
      } else {
        b.push(0);
      }
    });
    let BinImgData = group(b, 8);
    BinImgData.forEach((e, i) => {
      let bre = `${e[0]}${e[1]}${e[2]}${e[3]}`;
      let pre = `${e[4]}${e[5]}${e[6]}${e[7]}`;
      let newBre = setHex(bre);
      let newPre = setHex(pre);
      c += `0X${newBre}${newPre},`;
      if ((i + 1) % 16 === 0) c += "\n";
      return;
    });
    // console.log(c);

    fs.writeFile(__dirname + "/out.txt", c, { flag: "w" }, function (err) {
      if (err) {
        console.error(err);
      } else {
        console.log("写入成功");
      }
    });
  } catch (error) {
    console.log(error);
  }
}

const getPixelsPro = (formatImageUrl) => {
  return new Promise((resolve, reject) => {
    getPixels(formatImageUrl, function (err, pixels) {
      if (err) reject("Bad image path");
      resolve(pixels.data.slice());
    });
  });
};

const formatRGBAs = (imagePixel) => {
  return new Promise((resolve, reject) => {
    if (!imagePixel) reject("Bad image pixels");
    let a = imagePixel.map((e) => {
      if (e >= 190) return 1;
      return 0;
    });
    let b = group(a, 4);
    resolve(b);
  });
};

const group = (arr, num) => {
  num = num * 1 || 1;
  let ret = [];
  arr.forEach(function (item, i) {
    if (i % num === 0) {
      ret.push([]);
    }
    ret[ret.length - 1].push(item);
  });
  return ret;
};

const setBind = (key) => {
  switch (key) {
    case "00000000":
      return 0x00;
      break;
    case "00000001":
      return 0x01;
      break;
    case "00000010":
      return 0x02;
      break;
    case "00000011":
      return 0x03;
      break;
    case "00000100":
      return 0x04;
      break;
    case "00000101":
      return 0x05;
      break;
    case "00000110":
      return 0x06;
      break;
    case "00000111":
      return 0x07;
      break;
    case "00001000":
      return 0x08;
      break;
    case "00001001":
      return 0x09;
      break;
    case "00001010":
      return 0x0a;
      break;
    case "00001011":
      return 0x0b;
      break;
    case "00001100":
      return 0x0c;
      break;
    case "00001101":
      return 0x0d;
      break;
    case "00001110":
      return 0x0e;
      break;
    case "00001111":
      return 0x0f;
      break;
    case "00010000":
      return 0x10;
      break;
    case "00010001":
      return 0x11;
      break;
    case "00010010":
      return 0x12;
      break;
    case "00010011":
      return 0x13;
      break;
    case "00010100":
      return 0x14;
      break;
    case "00010101":
      return 0x15;
      break;
    case "00010110":
      return 0x16;
      break;
    case "00010111":
      return 0x17;
      break;
    case "00011000":
      return 0x18;
      break;
    case "00011001":
      return 0x19;
      break;
    case "00011010":
      return 0x1a;
      break;
    case "00011011":
      return 0x1b;
      break;
    case "00011100":
      return 0x1c;
      break;
    case "00011101":
      return 0x1d;
      break;
    case "00011110":
      return 0x1e;
      break;
    case "00011111":
      return 0x1f;
      break;
    case "00100000":
      return 0x20;
      break;
    case "00100001":
      return 0x21;
      break;
    case "00100010":
      return 0x22;
      break;
    case "00100011":
      return 0x23;
      break;
    case "00100100":
      return 0x24;
      break;
    case "00100101":
      return 0x25;
      break;
    case "00100110":
      return 0x26;
      break;
    case "00100111":
      return 0x27;
      break;
    case "00101000":
      return 0x28;
      break;
    case "00101001":
      return 0x29;
      break;
    case "00101010":
      return 0x2a;
      break;
    case "00101011":
      return 0x2b;
      break;
    case "00101100":
      return 0x2c;
      break;
    case "00101101":
      return 0x2d;
      break;
    case "00101110":
      return 0x2e;
      break;
    case "00101111":
      return 0x2f;
      break;
    case "00110000":
      return 0x30;
      break;
    case "00110001":
      return 0x31;
      break;
    case "00110010":
      return 0x32;
      break;
    case "00110011":
      return 0x33;
      break;
    case "00110100":
      return 0x34;
      break;
    case "00110101":
      return 0x35;
      break;
    case "00110110":
      return 0x36;
      break;
    case "00110111":
      return 0x37;
      break;
    case "00111000":
      return 0x38;
      break;
    case "00111001":
      return 0x39;
      break;
    case "00111010":
      return 0x3a;
      break;
    case "00111011":
      return 0x3b;
      break;
    case "00111100":
      return 0x3c;
      break;
    case "00111101":
      return 0x3d;
      break;
    case "00111110":
      return 0x3e;
      break;
    case "00111111":
      return 0x3f;
      break;
    case "01000000":
      return 0x40;
      break;
    case "01000001":
      return 0x41;
      break;
    case "01000010":
      return 0x42;
      break;
    case "01000011":
      return 0x43;
      break;
    case "01000100":
      return 0x44;
      break;
    case "01000101":
      return 0x45;
      break;
    case "01000110":
      return 0x46;
      break;
    case "01000111":
      return 0x47;
      break;
    case "01001000":
      return 0x48;
      break;
    case "01001001":
      return 0x49;
      break;
    case "01001010":
      return 0x4a;
      break;
    case "01001011":
      return 0x4b;
      break;
    case "01001100":
      return 0x4c;
      break;
    case "01001101":
      return 0x4d;
      break;
    case "01001110":
      return 0x4e;
      break;
    case "01001111":
      return 0x4f;
      break;
    case "01010000":
      return 0x50;
      break;
    case "01010001":
      return 0x51;
      break;
    case "01010010":
      return 0x52;
      break;
    case "01010011":
      return 0x53;
      break;
    case "01010100":
      return 0x54;
      break;
    case "01010101":
      return 0x55;
      break;
    case "01010110":
      return 0x56;
      break;
    case "01010111":
      return 0x57;
      break;
    case "01011000":
      return 0x58;
      break;
    case "01011001":
      return 0x59;
      break;
    case "01011010":
      return 0x5a;
      break;
    case "01011011":
      return 0x5b;
      break;
    case "01011100":
      return 0x5c;
      break;
    case "01011101":
      return 0x5d;
      break;
    case "01011110":
      return 0x5e;
      break;
    case "01011111":
      return 0x5f;
      break;
    case "01100000":
      return 0x60;
      break;
    case "01100001":
      return 0x61;
      break;
    case "01100010":
      return 0x62;
      break;
    case "01100011":
      return 0x63;
      break;
    case "01100100":
      return 0x64;
      break;
    case "01100101":
      return 0x65;
      break;
    case "01100110":
      return 0x66;
      break;
    case "01100111":
      return 0x67;
      break;
    case "01101000":
      return 0x68;
      break;
    case "01101001":
      return 0x69;
      break;
    case "01101010":
      return 0x6a;
      break;
    case "01101011":
      return 0x6b;
      break;
    case "01101100":
      return 0x6c;
      break;
    case "01101101":
      return 0x6d;
      break;
    case "01101110":
      return 0x6e;
      break;
    case "01101111":
      return 0x6f;
      break;
    case "01110000":
      return 0x70;
      break;
    case "01110001":
      return 0x71;
      break;
    case "01110010":
      return 0x72;
      break;
    case "01110011":
      return 0x73;
      break;
    case "01110100":
      return 0x74;
      break;
    case "01110101":
      return 0x75;
      break;
    case "01110110":
      return 0x76;
      break;
    case "01110111":
      return 0x77;
      break;
    case "01111000":
      return 0x78;
      break;
    case "01111001":
      return 0x79;
      break;
    case "01111010":
      return 0x7a;
      break;
    case "01111011":
      return 0x7b;
      break;
    case "01111100":
      return 0x7c;
      break;
    case "01111101":
      return 0x7d;
      break;
    case "01111110":
      return 0x7e;
      break;
    case "01111111":
      return 0x7f;
      break;
    case "10000000":
      return 0x80;
      break;
    case "10000001":
      return 0x81;
      break;
    case "10000010":
      return 0x82;
      break;
    case "10000011":
      return 0x83;
      break;
    case "10000100":
      return 0x84;
      break;
    case "10000101":
      return 0x85;
      break;
    case "10000110":
      return 0x86;
      break;
    case "10000111":
      return 0x87;
      break;
    case "10001000":
      return 0x88;
      break;
    case "10001001":
      return 0x89;
      break;
    case "10001010":
      return 0x8a;
      break;
    case "10001011":
      return 0x8b;
      break;
    case "10001100":
      return 0x8c;
      break;
    case "10001101":
      return 0x8d;
      break;
    case "10001110":
      return 0x8e;
      break;
    case "10001111":
      return 0x8f;
      break;
    case "10010000":
      return 0x90;
      break;
    case "10010001":
      return 0x91;
      break;
    case "10010010":
      return 0x92;
      break;
    case "10010011":
      return 0x93;
      break;
    case "10010100":
      return 0x94;
      break;
    case "10010101":
      return 0x95;
      break;
    case "10010110":
      return 0x96;
      break;
    case "10010111":
      return 0x97;
      break;
    case "10011000":
      return 0x98;
      break;
    case "10011001":
      return 0x99;
      break;
    case "10011010":
      return 0x9a;
      break;
    case "10011011":
      return 0x9b;
      break;
    case "10011100":
      return 0x9c;
      break;
    case "10011101":
      return 0x9d;
      break;
    case "10011110":
      return 0x9e;
      break;
    case "10011111":
      return 0x9f;
      break;
    case "10100000":
      return 0xa0;
      break;
    case "10100001":
      return 0xa1;
      break;
    case "10100010":
      return 0xa2;
      break;
    case "10100011":
      return 0xa3;
      break;
    case "10100100":
      return 0xa4;
      break;
    case "10100101":
      return 0xa5;
      break;
    case "10100110":
      return 0xa6;
      break;
    case "10100111":
      return 0xa7;
      break;
    case "10101000":
      return 0xa8;
      break;
    case "10101001":
      return 0xa9;
      break;
    case "10101010":
      return 0xaa;
      break;
    case "10101011":
      return 0xab;
      break;
    case "10101100":
      return 0xac;
      break;
    case "10101101":
      return 0xad;
      break;
    case "10101110":
      return 0xae;
      break;
    case "10101111":
      return 0xaf;
      break;
    case "10110000":
      return 0xb0;
      break;
    case "10110001":
      return 0xb1;
      break;
    case "10110010":
      return 0xb2;
      break;
    case "10110011":
      return 0xb3;
      break;
    case "10110100":
      return 0xb4;
      break;
    case "10110101":
      return 0xb5;
      break;
    case "10110110":
      return 0xb6;
      break;
    case "10110111":
      return 0xb7;
      break;
    case "10111000":
      return 0xb8;
      break;
    case "10111001":
      return 0xb9;
      break;
    case "10111010":
      return 0xba;
      break;
    case "10111011":
      return 0xbb;
      break;
    case "10111100":
      return 0xbc;
      break;
    case "10111101":
      return 0xbd;
      break;
    case "10111110":
      return 0xbe;
      break;
    case "10111111":
      return 0xbf;
      break;
    case "11000000":
      return 0xc0;
      break;
    case "11000001":
      return 0xc1;
      break;
    case "11000010":
      return 0xc2;
      break;
    case "11000011":
      return 0xc3;
      break;
    case "11000100":
      return 0xc4;
      break;
    case "11000101":
      return 0xc5;
      break;
    case "11000110":
      return 0xc6;
      break;
    case "11000111":
      return 0xc7;
      break;
    case "11001000":
      return 0xc8;
      break;
    case "11001001":
      return 0xc9;
      break;
    case "11001010":
      return 0xca;
      break;
    case "11001011":
      return 0xcb;
      break;
    case "11001100":
      return 0xcc;
      break;
    case "11001101":
      return 0xcd;
      break;
    case "11001110":
      return 0xce;
      break;
    case "11001111":
      return 0xcf;
      break;
    case "11010000":
      return 0xd0;
      break;
    case "11010001":
      return 0xd1;
      break;
    case "11010010":
      return 0xd2;
      break;
    case "11010011":
      return 0xd3;
      break;
    case "11010100":
      return 0xd4;
      break;
    case "11010101":
      return 0xd5;
      break;
    case "11010110":
      return 0xd6;
      break;
    case "11010111":
      return 0xd7;
      break;
    case "11011000":
      return 0xd8;
      break;
    case "11011001":
      return 0xd9;
      break;
    case "11011010":
      return 0xda;
      break;
    case "11011011":
      return 0xdb;
      break;
    case "11011100":
      return 0xdc;
      break;
    case "11011101":
      return 0xdd;
      break;
    case "11011110":
      return 0xde;
      break;
    case "11011111":
      return 0xdf;
      break;
    case "11100000":
      return 0xe0;
      break;
    case "11100001":
      return 0xe1;
      break;
    case "11100010":
      return 0xe2;
      break;
    case "11100011":
      return 0xe3;
      break;
    case "11100100":
      return 0xe4;
      break;
    case "11100101":
      return 0xe5;
      break;
    case "11100110":
      return 0xe6;
      break;
    case "11100111":
      return 0xe7;
      break;
    case "11101000":
      return 0xe8;
      break;
    case "11101001":
      return 0xe9;
      break;
    case "11101010":
      return 0xea;
      break;
    case "11101011":
      return 0xeb;
      break;
    case "11101100":
      return 0xec;
      break;
    case "11101101":
      return 0xed;
      break;
    case "11101110":
      return 0xee;
      break;
    case "11101111":
      return 0xef;
      break;
    case "11110000":
      return 0xf0;
      break;
    case "11110001":
      return 0xf1;
      break;
    case "11110010":
      return 0xf2;
      break;
    case "11110011":
      return 0xf3;
      break;
    case "11110100":
      return 0xf4;
      break;
    case "11110101":
      return 0xf5;
      break;
    case "11110110":
      return 0xf6;
      break;
    case "11110111":
      return 0xf7;
      break;
    case "11111000":
      return 0xf8;
      break;
    case "11111001":
      return 0xf9;
      break;
    case "11111010":
      return 0xfa;
      break;
    case "11111011":
      return 0xfb;
      break;
    case "11111100":
      return 0xfc;
      break;
    case "11111101":
      return 0xfd;
      break;
    case "11111110":
      return 0xfe;
      break;
    default:
      return 0xff;
      break;
  }
};

const setHex = (key) => {
  switch (key) {
    case "0001":
      return "1";
      break;
    case "0010":
      return "2";
      break;
    case "0011":
      return "3";
      break;
    case "0100":
      return "4";
      break;
    case "0101":
      return "5";
      break;
    case "0110":
      return "6";
      break;
    case "0111":
      return "7";
      break;
    case "1000":
      return "8";
      break;
    case "1001":
      return "9";
      break;
    case "1010":
      return "A";
      break;
    case "1011":
      return "B";
      break;
    case "1100":
      return "C";
      break;
    case "1101":
      return "D";
      break;
    case "1110":
      return "E";
      break;
    case "1111":
      return "F";
      break;
    default:
      return "0";
      break;
  }
};
