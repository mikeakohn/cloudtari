
To make these scripts work, they have to be in /var/www/html
and /etc/sudoers needs a line:

    www-data ALL=(ALL) NOPASSWD: /var/www/html/start_game.py

