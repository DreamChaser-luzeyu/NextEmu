#!/bin/python3

# import curses
# from curses import wrapper

# def show_title(win, msg):
#     win_rows, win_cols = win.getmaxyx()
#     win.addstr(0, (win_cols - len(msg)) // 2, msg)

# def create_win_left_half(full_win):
#     win_rows, win_cols = full_win.getmaxyx()
#     new_win = curses.newwin(win_rows - 2, win_cols // 2 - 2, 1, 1)
#     new_win.border(0)
#     return new_win
    


# def main(stdscr):
#     stdscr.clear()
#     stdscr.border(0)
#     show_title(stdscr, "Emulator")
#     stdscr.refresh()

#     new_win = create_win_left_half(stdscr)
#     show_title(new_win, "New win")
#     new_win.refresh()

#     stdscr.refresh()
#     new_win.refresh()

#     # print("????")

#     while True:
#         if stdscr.getch() == curses.KEY_RESIZE:
#             win_rows, win_cols = stdscr.getmaxyx()
#             stdscr.clear()
#             stdscr.resize(win_rows, win_cols)
#             stdscr.border(0)
#             stdscr.refresh()
#             show_title(stdscr, "Emulator")
            
#             new_win.clear()
#             new_win.refresh()
#             new_win.border(0)
#             show_title(new_win, "New win")

# if __name__ == '__main__':
#     wrapper(main)

import npyscreen
class TestApp(npyscreen.NPSApp):
    def main(self):
        F  = npyscreen.Form(name = "Welcome to Npyscreen",)
        t  = F.add(npyscreen.TitleText, name = "Text:",)
        fn = F.add(npyscreen.TitleFilename, name = "Filename:")
        fn2 = F.add(npyscreen.TitleFilenameCombo, name="Filename2:")
        dt = F.add(npyscreen.TitleDateCombo, name = "Date:")
        s  = F.add(npyscreen.TitleSlider, out_of=12, name = "Slider")
        ml = F.add(npyscreen.MultiLineEdit,
               value = """try typing here!\nMutiline text, press ^R to reformat.\n""",
               max_height=5, rely=9)
        ms = F.add(npyscreen.TitleSelectOne, max_height=4, value = [1,], name="Pick One",
                values = ["Option1","Option2","Option3"], scroll_exit=True)
        ms2= F.add(npyscreen.TitleMultiSelect, max_height =-2, value = [1,], name="Pick Several",
                values = ["Option1","Option2","Option3"], scroll_exit=True)

        my_entity = F.add(npyscreen.Button, max_height=1, name = "My button 1")
        my_entity2 = F.add(npyscreen.Button, max_height=1, name = "My button 2")

        # This lets the user interact with the Form.
        F.edit()

        print(ms.get_selected_objects())

if __name__ == "__main__":
    App = TestApp()
    App.run()