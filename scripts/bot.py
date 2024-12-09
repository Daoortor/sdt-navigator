import subprocess
import telebot
import os

from src.utils import mkpath_root


EXEC = mkpath_root('build/sdt-navigator')


with open(mkpath_root('scripts/token.txt')) as file:
    tg_token = file.read().strip()

bot = telebot.TeleBot(tg_token)

SELECTED_DATASET = {}  # {user_id: dataset_name}


def parse_message(message: str) -> str:
    return message.replace('—', '--')


def run_command(dataset: str, command: str) -> str:
    print(f'Running command: {command}')
    result = ""

    try:
        executable = [EXEC, dataset]
        # print(f'Running executable: {executable}')
        process = subprocess.Popen(
            executable,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        # print('Waiting for response')
        stdout, stderr = process.communicate(command, timeout=10)
        # print('Response received')

        if process.returncode == 0 and not stderr:
            result = stdout.split('[sdt-navigator]$')[1].strip()
        else:
            result = f'Error occurred: {stderr}'

        process.kill()

    except Exception as e:
        result = f'Error occurred: {e}'

    return result


@bot.message_handler(commands=['start'])
def start_message(message):
    try:
        print(f'Incoming message: {message.text}')

        bot.send_message(
            message.chat.id,
            'Please select a dataset using /select command (e.g., `/select gtfs_hamburg`)',
            parse_mode='Markdown'
        )
    except Exception:
        pass


@bot.message_handler(commands=['select'])
def select_dataset(message):
    try:
        print(f'Incoming message: {message.text}')

        user_input = message.text.strip().removeprefix('/select').strip()

        if not user_input:
            return start_message(message)

        if not os.path.isdir(mkpath_root(f'data/{user_input}')):
            bot.send_message(
                message.chat.id,
                'Dataset was not found. Please try again',
            )
            return start_message(message)

        SELECTED_DATASET[message.chat.id] = user_input
        bot.send_message(
            message.chat.id,
            f'Dataset \"{user_input}\" selected! You can now use /route or /search like this:'
        )
        bot.send_message(
            message.chat.id,
            f'```\n'
            f'/route --start \"Bremen Hbf\" --end \"Hamburg Airport (Flughafen)\" --date 2020-03-20 --time 06:00\n'
            f'/search Bremen\n'
            f'```',
            parse_mode='Markdown'
        )
    except Exception:
        pass


@bot.message_handler(commands=['route'])
def route_command(message):
    try:
        print(f'Incoming message: {message.text}')

        if message.chat.id not in SELECTED_DATASET:
            return start_message(message)

        user_input = message.text.strip().removeprefix('/')

        user_input = parse_message(user_input)

        result = run_command(SELECTED_DATASET[message.chat.id], user_input)

        bot.send_message(message.chat.id, result)
    except Exception:
        pass


@bot.message_handler(commands=['search'])
def search_command(message):
    try:
        print(f'Incoming message: {message.text}')

        if message.chat.id not in SELECTED_DATASET:
            return start_message(message)

        user_input = message.text.strip().removeprefix('/')

        result = run_command(SELECTED_DATASET[message.chat.id], user_input)

        if result[result.find('|'):].strip():
            result = result[:result.find('|')] + '```\n' + result[result.find('|'):] + '\n```'
        else:
            result = f'No stations found'

        print(f'Sending response:\n{result}')

        bot.send_message(message.chat.id, result, parse_mode='Markdown')
    except Exception:
        pass


@bot.message_handler(func=lambda message: True)
def handle_input(message):
    try:
        print(f'Incoming message: {message.text}')

        bot.send_message(
            message.chat.id,
            'Please use commands: /select, /route, /search'
        )
    except Exception:
        pass


print('Starting Telegram Bot...')
bot.polling()
