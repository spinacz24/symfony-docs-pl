.. index::
    single: bezpieczeństwo; podszywanie sie pod użytkownika

Jak podszywać się po użytkownika
================================

Czasami zachodzi potrzeba przełączenia się z jednego użytkownika na innego,
bez konieczności wylogowywania się i ponownego logowania (na przykład podczas
debugowania lub próby poznania błędów, jakie widzi użytkownik w swojej sesji
a których nie można odtworzyć inaczej). Można to łatwo zrobić przez aktywowanie
detektora (*ang. listener*) zapory ``switch_user``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    switch_user: true

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <switch-user />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main'=> array(
                    // ...
                    'switch_user' => true,
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

Podczas podszywania sie, użytkownik jest dostarczany ze specjalną rolą o nazwie
``ROLE_PREVIOUS_ADMIN``. Na przykład, w szablonie rola ta może być używana do
wyświetlania odnośnika do opuszczenia podszywania się:

.. configuration-block::

    .. code-block:: html+jinja

        {% if is_granted('ROLE_PREVIOUS_ADMIN') %}
            <a href="{{ path('homepage', {'_switch_user': '_exit'}) }}">Exit impersonation</a>
        {% endif %}

    .. code-block:: html+php

        <?php if ($view['security']->isGranted('ROLE_PREVIOUS_ADMIN')): ?>
            <a
                href="<?php echo $view['router']->generate('homepage', array(
                    '_switch_user' => '_exit',
                ) ?>"
            >
                Exit impersonation
            </a>
        <?php endif ?>

W niektórych przypadkach może zajść potrzeba pobrania obiektu, który reprezentuje
naśladowanego użytkownika zamiast tego, który podszył się pod tego użytkownika.
Trzeba użyć nastęþującego fragmentu do iteracji po rolach użytkowników, aż znajdzie
się jeden taki obiekt ``SwitchUserRole``::

    use Symfony\Component\Security\Core\Role\SwitchUserRole;

    $authChecker = $this->get('security.authorization_checker');
    $tokenStorage = $this->get('security.token_storage');

    if ($authChecker->isGranted('ROLE_PREVIOUS_ADMIN')) {
        foreach ($tokenStorage->getToken()->getRoles() as $role) {
            if ($role instanceof SwitchUserRole) {
                $impersonatingUser = $role->getSource()->getUser();
                break;
            }
        }
    }

Oczywiście funkcjonalność ta może być udostępniona tylko wąskiej grupie użytkowników.
Domyślnie dostęp jest zastrzeżony dla użytkowników posiadających rolę
``ROLE_ALLOWED_TO_SWITCH``. Nazwa tej roli może być zmodyfikowana w ustawieniu
``role``. W celu zapewnienia dodatkowego bezpieczeństwa można również zmienić
nazwę parametru zapytania  w ustawieniu ``parameter``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    switch_user: { role: ROLE_ADMIN, parameter: _want_to_be_this_user }

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">
            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <switch-user role="ROLE_ADMIN" parameter="_want_to_be_this_user" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

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
        
Zdarzenia
---------

Zapora wywołuje zdarzenie ``security.switch_user`` zaraz po zakończeniu podszywania.
Klasa :class:`Symfony\\Component\\Security\\Http\\Event\\SwitchUserEvent` jest
przekazywana do detektota i może być używana do pobierania użytkownika, pod którego
się podszywa.

Tak jak opisano to w artykule 
:doc:`Wykonywanie "lepkich" ustawień narodowych podczas sesji użytkownika </cookbook/session/locale_sticky_session>`
nie aktualizuje ustawień narodowych podczas podszywania się pod użytkownika.
Poniższy przykładowy kod pokazuje, jak można zmieniać lepkie ustawienia narodowe:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.switch_user_listener:
                class: AppBundle\EventListener\SwitchUserListener
                tags:
                    - { name: kernel.event_listener, event: security.switch_user, method: onSwitchUser }

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >
            <services>
                <service id="app.switch_user_listener"
                    class="AppBundle\EventListener\SwitchUserListener"
                >
                    <tag name="kernel.event_listener"
                        event="security.switch_user"
                        method="onSwitchUser"
                    />
                </service>
            </services>
        </container>

    .. code-block:: php

        // app/config/services.php
        $container
            ->register('app.switch_user_listener', 'AppBundle\EventListener\SwitchUserListener')
            ->addTag('kernel.event_listener', array('event' => 'security.switch_user', 'method' => 'onSwitchUser'))
        ;

.. caution::

    Implementacja detektora zakłada, że encja ``User`` ma metodę ``getLocale()``.

.. code-block:: php

        // src/AppBundle/EventListener/SwitchUserListener.pnp
        namespace AppBundle\EventListener;

        use Symfony\Component\Security\Http\Event\SwitchUserEvent;

        class SwitchUserListener
        {
            public function onSwitchUser(SwitchUserEvent $event)
            {
                $event->getRequest()->getSession()->set(
                    '_locale',
                    $event->getTargetUser()->getLocale()
                );
            }
        }        
