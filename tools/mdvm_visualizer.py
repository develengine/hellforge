import sys
import tkinter as tk

args = sys.argv[1:]

bs = [ ]

# print(int.from_bytes(bs, byteorder='big', signed=False))
# print(int.from_bytes(bs, byteorder='little', signed=False))

for i in args:
    print(i)

root = tk.Tk()

# canvas = tk.Canvas(root, width=)

root.mainloop()
