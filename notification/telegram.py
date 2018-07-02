import telepot

from .main import Notification


class TelegramNotification(Notification):
    """Class which notify via Telegram"""

    def __init__(self, token, chat_id, logger):
        Notification.__init__(self, token, chat_id, logger)
        self.bot = telepot.Bot(self.token)

    def notify(self, user, message):
        self.bot.sendMessage(user, message)
        self.logger.debug('Message {} send to user {}'.format(message, user))

    def broadcast(self, message):
        self.logger.debug("Initialized Broadcast")
        for identification in self.chat_id_list:
            self.bot.sendMessage(identification, message)
            self.logger.debug("Message {} send to {}".format(message, identification))
        self.logger.debug("Finished broadcasting")
