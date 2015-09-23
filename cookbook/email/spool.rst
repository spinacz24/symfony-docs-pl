.. index::
   single: wiadomości email; kolejkowanie
   single: poczta elektroniczna; kolejkowanie

Jak kolejkować wiadomości email
===============================

Jeśli do wysyłania wiadomości email z aplikacji Symfony używa się pakietu
SwiftmailerBundle, wiadomości będą domyślnie wysyłane natychmiast.
Może to jednak spowodować problem wydajności w komunikacji pomiędzy Swift Mailer
a transportem poczty, skutkujace tym, że użytkownik będzie musiał czekać
na kolejną stronę zanim wiadomość zostanie wysłana. Problem ten można wyeliminować
ustawiając "kolejkowanie" wiadomości w miejsce natychmiastowego ich wysyłania.
Będzie to oznaczać, że Swift Mailer nie będzie
próbował od wysłać wiadomość, ale zapisze ją w jakimś buforze, na przykład w pliku.
Teraz, inny proces może odczytywać z bufora zapisane tam wiadomości i je
wysłać. Obecnie Swift Mailer obsługuje tylko kolejkowanie do pliku lub pamięci.

Kolejkowanie z użyciem pamięci
------------------------------

Podczas wykorzystywania pamięci do kolejkowania wiadomości email, zostaną one
wysłane po kolei zanim kernel zakończy działanie. Oznacza to, że wiadomość
zostanie wysłana, tylko jeśli całość żądania została wykonana bez nieobsługiwanych
wyjatków lub jakichś błędów. W celu skonfigurowania opcji Swift Mailer z obsługą
pamięci trzeba ustawić następującą konfigurację:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            # ...
            spool: { type: memory }

    .. code-block:: xml

        <!-- app/config/config.xml -->

        <!--
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            http://symfony.com/schema/dic/swiftmailer
            http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config>
             <swiftmailer:spool type="memory" />
        </swiftmailer:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
             // ...
            'spool' => array('type' => 'memory')
        ));

Kolejkowanie z użyciem pliku
----------------------------

W celu wykorzystywania kolejkowania z uzyciem pliku trzeba ustawić następujacą
konfigurację:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            # ...
            spool:
                type: file
                path: /path/to/spool

    .. code-block:: xml

        <!-- app/config/config.xml -->

        <!--
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            http://symfony.com/schema/dic/swiftmailer
            http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config>
             <swiftmailer:spool
                 type="file"
                 path="/path/to/spool" />
        </swiftmailer:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
             // ...

            'spool' => array(
                'type' => 'file',
                'path' => '/path/to/spool',
            ),
        ));

.. tip::

    Jeśli chcesz zapisać plik kolejkowania gdzieś w katalogu projektu, to pamiętaj,
    że można wykorzystywać parametr ``%kernel.root_dir%`` jako odniesienie do
    katalogu głównego projektu:

    .. code-block:: yaml

        path: "%kernel.root_dir%/spool"

Teraz, gdy aplikacjia wysyła wiadomość, to nie jest ona wysyłana, ale zapisywana
do bufora. Wysyłanie wiadomości z bufora jest dokonywane oddzielnie. Oto polecenie
konsolowe powodujace wysłanie wiadomości z bufora:

.. code-block:: bash

    $ php app/console swiftmailer:spool:send --env=prod

Jest możliwość ograniczenia ilości komunikatów do wysłania:

.. code-block:: bash

    $ php app/console swiftmailer:spool:send --message-limit=10 --env=prod

Można też ustawić limit czasu w sekundach:

.. code-block:: bash

    $ php app/console swiftmailer:spool:send --time-limit=10 --env=prod

Oczywiście że w rzeczywistości nie uruchamia sie tego polecenia ręcznie.
Zamiast tego, powyzsze polecenie konsolowe powinno być uruchamiane przez zadanie
crona lub zadania harmonogramu (w Windowsie) w regularnych odstęþach czasu.
