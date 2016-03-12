.. index::
   single: bezpieczeństwo; dostawca użytkowników

Jak stosować wielu dostawców użytkowników
=========================================

Każdy mechanizm uwierzytelniania (np. uwierzytelnianie HTTP, logowanie formularzowe
itd.) używa dokładnie jednego dostawcę użytkowników i domyślnie stosuje pierwszego
zadeklarowanego dostawcę. Co więc zrobić, jeśli istnieje potrzeba stosowania
różnego rodzaju dostawców użytkowników (np. dla kilku użytkowników dostawcę
"z pamięci" a dla pozostałych dostawcę z bazy danych)? Jest to możliwe, poprzez
utworzenie nowego dostawcy, który łączy razem dwóch innych dostawców:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            providers:
                chain_provider:
                    chain:
                        providers: [in_memory, user_db]
                in_memory:
                    memory:
                        users:
                            foo: { password: test }
                user_db:
                    entity: { class: Acme\UserBundle\Entity\User, property: username }

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
                <provider name="chain_provider">
                    <chain>
                        <provider>in_memory</provider>
                        <provider>user_db</provider>
                    </chain>
                </provider>

                <provider name="in_memory">
                    <memory>
                        <user name="foo" password="test" />
                    </memory>
                </provider>

                <provider name="user_db">
                    <entity class="Acme\UserBundle\Entity\User" property="username" />
                </provider>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'providers' => array(
                'chain_provider' => array(
                    'chain' => array(
                        'providers' => array('in_memory', 'user_db'),
                    ),
                ),
                'in_memory' => array(
                    'memory' => array(
                       'users' => array(
                           'foo' => array('password' => 'test'),
                       ),
                    ),
                ),
                'user_db' => array(
                    'entity' => array(
                        'class' => 'Acme\UserBundle\Entity\User',
                        'property' => 'username',
                    ),
                ),
            ),
        ));

Teraz, wszystkie mechanizmy uwierzytelniania będą używać dostawcę ``chain_provider``,
ponieważ jest on zdefiniowany jako pierwszy. Dostawca ``chain_provider`` będzie
z kolei ładować użytkowników ``in_memory`` i ``user_db``.

Można również skonfigurować zaporę lub pojedynczy mechanizm uwierzytelniania, tak
aby można było używać określonego dostawcę. Tu tak samo używany jest pierwszy
dostawca, chyba że jawnie określono to inaczej:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                secured_area:
                    # ...
                    pattern: ^/
                    provider: user_db
                    http_basic:
                        realm: "Secured Demo Area"
                        provider: in_memory
                    form_login: ~

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
                <firewall name="secured_area" pattern="^/" provider="user_db">
                    <!-- ... -->
                    <http-basic realm="Secured Demo Area" provider="in_memory" />
                    <form-login />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'pattern' => '^/',
                    'provider' => 'user_db',
                    'http_basic' => array(
                        // ...
                        'realm' => 'Secured Demo Area',
                        'provider' => 'in_memory',
                    ),
                    'form_login' => array(),
                ),
            ),
        ));

W tym przykładzie, jeśli użytkownik próbuje zalogować się poprzez uwierzytelnianie
HTTP, system uwierzytelniania zastosuje dostawcę ``in_memory``. Jeśli jednak
użytkownik spróbuje zalogować się poprzez mechanizm logowania formularzowego,
zastosowany zostanie dostawca ``user_db`` (ponieważ jest on domyślny w całości
dla tej zapory).

Więcej informacji o dostawcy użytkowników i konfigurowaniu zapory można znaleźć
w artykule :doc:`/reference/configuration/security`.
