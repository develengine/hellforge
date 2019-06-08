import sys
import tkinter as tk

args = sys.argv[1:]
span = 10

def read_file_from(dr, s, n):
    with open(dr, "rb") as f:
        f.seek(s)
        return f.read(n)

label_path = ""
image_path = ""

count = 0
im_width = 0
im_height = 0
size = 0

for arg in args:
    if arg.startswith("-l"):
        label_path = arg[2:]
    elif arg.startswith("-i"):
        image_path = arg[2:]


with open(image_path, "rb") as f:
    f.seek(4) # magic number
    count = int.from_bytes(f.read(4), byteorder='big', signed=False)
    im_width = int.from_bytes(f.read(4), byteorder='big', signed=False)
    im_height = int.from_bytes(f.read(4), byteorder='big', signed=False)
    size = im_width * im_height

root = tk.Tk()
root.title("MDVM Visualizer")
root.resizable(width=False, height=False)

canvas = tk.Canvas(root, width=im_width * span, height=im_height * span)
canvas.pack()

entry = tk.Entry(root)
entry.pack()

label_text = tk.StringVar()
text_label = tk.Label(root, textvariable=label_text)
text_label.pack()

def draw(n):
    image = list(read_file_from(image_path, 16 + (size * n), size))
    canvas.delete("all")
    canvas.create_rectangle(0, 0, im_width * span, im_height * span, fill="#000000")
    for y in range(im_height):
        for x in range(im_width):
            shade = format(image[y * im_width + x], '02x').upper()
            color = '#' + shade + shade + shade
            x_pos = x * span
            y_pos = y * span
            canvas.create_rectangle(x_pos, y_pos, x_pos + span, y_pos + span, fill=color)

def enter_callback():
    try:
        number = int(entry.get())
        draw(number)
        label = read_file_from(label_path, 8 + number, 1)
        label_text.set(str(ord(label)))
    except Exception:
        return

enter_callback()

button = tk.Button(root, text="text", command = enter_callback)
button.pack()

root.mainloop()
