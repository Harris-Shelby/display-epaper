// const pngtolcd = require("png-to-lcd");
const floydSteinberg = require("floyd-steinberg");
const fs = require("fs");
const sharp = require("sharp");
const PNG = require("pngjs").PNG;
const getPixels = require("get-pixels");

resizeImage("abb.jpg");

async function resizeImage(imageData) {
  try {
    await sharp(imageData)
      .resize({
        width: 128,
        height: 296,
      })
      .toFormat("jpeg")
      .png({ quality: 90 })
      .toFile(`./out.png`);

    await fs
      .createReadStream("out.png")
      .pipe(new PNG())
      .on("parsed", function () {
        floydSteinberg(this).pack().pipe(fs.createWriteStream("out1.png"));
      });

    let pixelData = await getPixelsPro("./out1.png");
    let newPixel = await formatRGBAs(pixelData);
    let b = [];
    let c = "";
    newPixel.forEach((e, i) => {
      if (`${e.join("")}` === "1111") {
        b.push(1);
      } else {
        b.push(0);
      }
      // if ((i + 1) % 128 === 0) b += "\n";
      // return;
    });
    let newPixel111 = group(b, 8);
    newPixel111.forEach((e, i) => {
      c += `${e.join("")},`;
      if ((i + 1) % 16 === 0) c += "\n";
      return;
    });
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
      if (e === 255) return 1;
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
