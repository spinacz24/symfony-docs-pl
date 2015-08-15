.. index::
    single: bezpieczeństwo; personifikacja użytkownika

Jak spersonifikować użytkownika
===============================

Czasami zachodzi potrzeba przełączenia się z jednego użytkownika na innego,
bez konieczności wylogowywania się i ponownego logowania (na przykład podczas
debugowania lub próby poznania błędów, jakie widzi użytkownik w swojej sesji
a których nie można odtworzyć inaczej). Można to łatwo zrobić przez aktywowanie
podsłuchiwacza zapory ``switch_user``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                main:
                    # ...
                    switch_user: true

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
                <firewall>
                    <!-- ... -->
                    <switch-user />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => true
                ),
            ),
        ));

Aby przełączyć się na innego użytkownika, wystarczy dodać łańcuch zapytania
z parametrem ``_switch_user`` i nazwą użytkownika jako wartością tego parametru
w bieżącym adresie URL:

.. code-block:: text

    http://example.com/somewhere?_switch_user=thomas

Aby przełączyć się z powrotem na oryginalnego użytkownika, trzeba użyć specjanej
nazwy użytkownika, ``_exit``:

.. code-block:: text

    http://example.com/somewhere?_switch_user=_exit

Podczas personifikacji, użytkownik jest dostarczany ze specjalną rolą o nazwie
``ROLE_PREVIOUS_ADMIN``. W szablonie, na przykład, rola ta może być używana do
wyświetlania łączy do istniejących personifikacji:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {% if is_granted('ROLE_PREVIOUS_ADMIN') %}
            <a href="{{ path('homepage', {'_switch_user': '_exit'}) }}">Exit impersonation</a>
        {% endif %}

    .. code-block:: html+php
       :linenos:

        <?php if ($view['security']->isGranted('ROLE_PREVIOUS_ADMIN')): ?>
            <a
                href="<?php echo $view['router']->generate('homepage', array(
                    '_switch_user' => '_exit',
                ) ?>"
            >
                Exit impersonation
            </a>
        <?php endif; ?>

Oczywiście funkcjonalność ta może być udostępnione tylko wąskiej grupie użytkowników.
Domyślnie dostęp jest zastrzeżony dla użytkowników posiadających rolę
``ROLE_ALLOWED_TO_SWITCH``. Nazwa tej roli może być zmodyfikowana w ustawieniu
``role``. W celu zapewnienia dodatkowego bezpieczeństwa można również zmienić
nazwę parametru zapytania  w ustawieniu ``parameter``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            firewalls:
                main:
                    # ...
                    switch_user: { role: ROLE_ADMIN, parameter: _want_to_be_this_user }

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
                <firewall>
                    <!-- ... -->
                    <switch-user role="ROLE_ADMIN" parameter="_want_to_be_this_user" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => array(
                        'role' => 'ROLE_ADMIN',
                        'parameter' => '_want_to_be_this_user',
                    ),
                ),
            ),
        ));
