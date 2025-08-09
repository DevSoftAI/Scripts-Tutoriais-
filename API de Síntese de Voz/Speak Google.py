# -*- coding: utf-8 -*-
import winreg
import sys
import os
import asyncio
from gtts import gTTS
import pygame
import time
import psutil

#se essa API tiver aberta mais de uma vez, então a encerre.
count = 0
for proc in psutil.process_iter(['name']):
    try:
        if proc.info['name'] and "Speak Google" in proc.info['name']:
            count += 1
    except:
        pass

if count >= 2:
    os._exit(0)


# Caminho do registro
REG_PATH = r'JARVIS'
OUTPUT_FILE = 'C:\\JARVIS\\JARVIS\\APIs\\Temp\\Speak Google.mp3'

# Funções para manipular o registro do Windows
def set_reg(name, value):
    try:
        winreg.CreateKey(winreg.HKEY_CURRENT_USER, REG_PATH)
        registry_key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_PATH, 0, winreg.KEY_WRITE)
        winreg.SetValueEx(registry_key, name, 0, winreg.REG_SZ, value)
        winreg.CloseKey(registry_key)
        return True
    except WindowsError:
        return False

def get_reg(name):
    try:
        registry_key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, REG_PATH, 0, winreg.KEY_READ)
        value, regtype = winreg.QueryValueEx(registry_key, name)
        winreg.CloseKey(registry_key)
        return value
    except WindowsError:
        return None

# Configura tipo e lista de vozes do gTTS
set_reg('Speak Type', 'Speak Google.exe')
set_reg('Speak List','en|es|pt') #vozes ou idiomas separados com |

def play_audio(file):
    try:
        pygame.mixer.init()
        pygame.mixer.music.load(file)
        pygame.mixer.music.play()
        set_reg('Speak Status', '1')

        while pygame.mixer.music.get_busy():
            speak_status = get_reg('Speak Status')

            if speak_status == '5':  # parar
                pygame.mixer.music.stop()
                set_reg('Speak Status', '0')
                break
            elif speak_status == '4':  # pausar
                pygame.mixer.music.pause()
                while get_reg('Speak Status') != '3':  # aguarda "continuar"
                    time.sleep(0.1)
                pygame.mixer.music.unpause()

        set_reg('Speak Status', '0')
        set_reg('Speak String', '')
        pygame.mixer.quit()

        if os.path.exists(file):
            os.remove(file)
    except:
        os._exit(0)

async def generate_and_play():
    while True:
        speak_string = get_reg('Speak String')
        speak_voice = get_reg('Speak Voice ID')
        if speak_string and speak_string.strip():
            try:
                tts = gTTS(text=speak_string, lang=speak_voice)
                tts.save(OUTPUT_FILE)
            except:
                await asyncio.sleep(1)
                continue

            play_audio(OUTPUT_FILE)

# Loop assíncrono
loop = asyncio.get_event_loop()
loop.run_until_complete(generate_and_play())


