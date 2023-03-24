const Canvas = require("canvas");
const fs = require("fs");

async function main() {
  // 设置字符和字体
  const chars = "abcdefghijklmnopqrstuvwxyz";
  const font = "Arial";

  // 设置字体大小和画布大小
  const fontSize = 30;
  const canvasWidth = 40;
  const canvasHeight = 40;

  // 创建画布和上下文
  const canvas = Canvas.createCanvas(canvasWidth, canvasHeight);
  const ctx = canvas.getContext("2d");

  // 设置字体和字体大小
  ctx.font = `${fontSize}px ${font}`;
  ctx.fillStyle = "#000";

  // 创建位图数组
  const bitmaps = {};

  // 循环处理每个字符
  for (let i = 0; i < chars.length; i++) {
    const char = chars[i];

    // 清空画布
    ctx.clearRect(0, 0, canvasWidth, canvasHeight);

    // 在画布上绘制字符
    ctx.fillText(char, 0, fontSize);

    // 等待画布绘制完成
    await new Promise((resolve) => setTimeout(resolve, 0));

    // 获取位图数据
    const imageData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);
    console.log(imageData);
    // 将位图数据转换为位图数组
    const bitmap = [];

    for (let y = 0; y < canvasHeight; y++) {
      for (let x = 0; x < canvasWidth; x++) {
        const index = (y * canvasWidth + x) * 4;
        const r = imageData.data[index];
        const g = imageData.data[index + 1];
        const b = imageData.data[index + 2];

        // 根据像素的灰度值设置位图数组
        const gray = Math.round(0.299 * r + 0.587 * g + 0.114 * b);
        bitmap.push(gray < 128 ? 0 : 1);
      }
    }

    bitmaps[char] = bitmap;
  }

  // 将位图数组保存到文件
  fs.writeFileSync("bitmap.json", JSON.stringify(bitmaps));
}

main();
