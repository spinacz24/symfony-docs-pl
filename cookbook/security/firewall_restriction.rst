.. index::
   single: bezpieczeństwo; zapora
   single: zapora; ograniczanie

Jak ograniczyć zaporę bezpieczeństwa do określonego żądania
===========================================================

Komponent Security umożliwia tworzenie zapór, które dopasowują określone opcje
żądania. W większości przypadków dopasowywanie ścieżek URL jest wystarczające,
ale w szczególnych przypadkach możliwe jest szczegółowsze ograniczenie inicjalizacji
zapory uwzględniające inne opcje żądania.

.. note::

    Można zastosować dowolne ograniczenie omawiane w tym artykule lub mieszać je
    w celu osiągnięcia pożądanej konfiguracji zapory. 

Ograniczenie przez wzorzec
--------------------------

Jest to domyślne ograniczenie, które ogranicza zaporę, by była inicjowana tylko
jeśli ściezka URL żądania zgadza się ze wzorcem skonfigurowanym w opcji ``pattern``. 

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml

        # ...
        security:
            firewalls:
                secured_area:
                    pattern: ^/admin
                    # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" pattern="^/admin">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php

        // ...
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'pattern' => '^/admin',
                    // ...
                ),
            ),
        ));

Opcja ``pattern`` jest wyrażeniem regularnym. W naszym przykładzie, zapora może
zostać aktywowana, gdy ścieżka URL rozpoczyna się (ze względu na znak ``^``) od
``/admin``. Jeśli ścieżka URL nie zgadza się z tym wzorcem, zapora nie zostanie
aktywowana i następne zapory uzyskują możliwość dopasowania tego żądania.

Ograniczenie przez host
-----------------------

Jeśli porównanie ze wzorcem ``pattern`` nie jest wystarczające, żądanie można
porównywać z wartością opcji ``host``.
Gdy ustawiona jest wartość opcji ``host``, zapora będzie aktywowana tylko jeśli 
host żądania pasuje do wartości tej opcji konfiguracyjnej.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml

        # ...
        security:
            firewalls:
                secured_area:
                    host: ^admin\.example\.com$
                    # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" host="^admin\.example\.com$">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php

        // ...
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'host' => '^admin\.example\.com$',
                    // ...
                ),
            ),
        ));

Wartość ``host`` (podobnie jak ``pattern``) jest wyrażeniem regularnym. W naszym
przykładzie, zapora będzie aktywowana tylko, jeśli host jest dokładnie równy
(ze wzgledu na znaki ``^`` i ``$`` zawarte we wzorcu) z nazwą hosta ``admin.example.com``.
Jeśli nazwa hosta nie pasuje do wzorca, zapora nie będzie aktywowana i następne
zapory uzyskują możliwość dopasowania żądania.

Ograniczenie przez metody HTTP
------------------------------

Opcja konfiguracyjna ``methods`` ogranicza zaporę do określonych metod HTTP.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml

        # ...
        security:
            firewalls:
                secured_area:
                    methods: [GET, POST]
                    # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" methods="GET,POST">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php

        // ...
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    'methods' => array('GET', 'POST'),
                    // ...
                ),
            ),
        ));

W tym przykładzie, zapora jest aktywowana tylko, jeśli metodą HTTP żądania jest
``GET`` albo ``POST``. Jeśli metoda jest inna niż, określono to w tablicy metod,
zapora nie zostanie aktywowana i następne zapory uzyskuja możliwość dopasowania
żądania.
