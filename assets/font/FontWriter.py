import sys
from PIL import Image, ImageDraw, ImageFont

def write_font_to_image(font: ImageFont, image: Image, width: int):
    draw = ImageDraw.Draw(image)
    for i in range(0, 256):
        x = (i % 16) * width
        y = (i // 16) * width
        draw.text((x, y), chr(i), font=font, fill=(255, 255, 255))
    image.convert("1")

def write_image_to_binary(image: Image, file_name: str):
    output_file = open(file_name, "wb")
    for y in range(0, image.height):
        for x in range(0, image.width):
            pixel = image.getpixel((x, y))
            if pixel[0] == 0:
                output_file.write(bytes([0, 0, 0]))
            else:
                output_file.write(bytes([255, 255, 255]))
    output_file.close()

def __main__():
    # Check args
    if len(sys.argv) != 3:
        print("Usage: python FontWriter.py <font_file> <font_width>")
        return
    
    # Get font file from args
    font_file = sys.argv[1]
    font_width = int(sys.argv[2])

    # Extract font name from file
    font_name = font_file.split("/")[-1].split(".")[0]

    image = Image.new("RGB", (font_width * 16, font_width * 16), (0, 0, 0))
    font = ImageFont.truetype(font_file, font_width)

    write_font_to_image(font, image, font_width)
    write_image_to_binary(image, f"{font_name}.bin")
    image.save(f"{font_name}.png")

if __name__ == "__main__":
    __main__()
