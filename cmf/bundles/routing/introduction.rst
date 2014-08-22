.. index::
    single: trasowanie; pakiety
    single: RoutingBundle

RoutingBundle
=============

    Pakiet `RoutingBundle`_ integruje dynamiczne trasowanie z Symfony przy użyciu
    komponentu CMF Routing. Proszę zapoznac się z :doc:`dokumentacją komponentu
    <../../components/routing/introduction>` aby zapoznać się ze szczegółami
    implementacji usług objaśnianych w tym rozdziale.

``ChainRouter`` zastępuje domyślny router Symfony. Routery znajdujące się w łańcuchu
są zarządzanie przy pomocy priorytetowej listy roterów. Podejmowana jest
próba dopasowania tras do żądań i wygenerowania ścieżek URL z każdej z tras.
Jeden z routerów w łańcuchu może być oczywiście wskazany jako router domyślny,
tak więc można wciąż wykorzystywać standardowy sposób Symfony2 określania tras,
gdy ma to sens.

Dodatkowo pakiet ten dostarcza użyteczne implementacje routera. Zapewnia ``DynamicRouter``,
który trasuje wykorzystując własną logikę ładowania obiektów  Route Symfony2.
Dostawca ten być implementowany z wykorzystaniem bazy danych. Pakiet dostarcza
domyślne implementacje Doctrine PHPCR-ODM i Doctrine ORM.

Usługa DynamicRouter jest dostępna tylko po jawnym włączeniu jej w konfiguracji
aplikacji.

Na koniec, pakiet ten dostarcza dokumenty tras dla Doctrine PHPCR-ODM i ORM, jak
też kontroler dla przekierowywania tras.

Instalacja
----------

Pakie ten można zainstalować `przy pomocy Composera`_ wykorzystując pakiet 
`symfony-cmf/routing-bundle`_.

.. index:: RoutingBundle
.. index::
   single: trasowanie; ChainRouter

ChainRouter
-----------

ChainRouter może zastąpić domyślny system trasowania na implementację udostępniającą
łańcuch routerów. Nie trasuje on niczego samodzielnie, lecz tylko zapewnia wykonywanie
pętli po połączonych routerach. Dla obsłużenia standardowej konfigurację tras Symfony,
można wstawić do łańcucha domyślny router Symfony.

Można skonfigurować usługi trasowania, tak aby można było je używać w łańcuchu,
zobacz :ref:`reference-config-routing-chain_routers`.

.. _routing-chain-router-tag:

Ładowanie tras z tagowaniem
---------------------------

Można użyć tag usługi ``router`` do automatycznego zarejestrowania tras.
Tag ten ma opcjonalny atrybut ``priority``. Większy priorytet powoduje późniejsze
odpytanie routera o dopasowanie trasy. Jeśli nie określi się priorytetu, to router
ten zostanie umieszczony na końcu listy. Jeśli istnieje kilka routerów z tym samym
priorytetem, to kolejność między nimi jest nieokreślona. Tagowana usługa może wyglądać
następująco:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        services:
            acme_core.my_router:
                class: "%my_namespace.my_router_class%"
                tags:
                    - { name: router, priority: 300 }

    .. code-block:: xml
       :linenos:

        <service id="acme_core.my_router" class="%my_namespace.my_router_class%">
            <tag name="router" priority="300" />
            <!-- ... -->
        </service>

    .. code-block:: php
       :linenos:

        $container
            ->register('acme_core.my_router', '%acme_core.my_router')
            ->addTag('router', array('priority' => 300))
        ;

Proszę zapoznać się również z oficjalną `dokumentacją Symfony2 dla tagów DependencyInjection`_

Rozdziały
---------

* :doc:`dynamic`
* :doc:`dynamic_customize`

Dalsza lektura
--------------

Dla zapoznania się z informacjami o trasowaniu w Symfony CMF, prosimy zapoznać się z :

* dokumentacją :doc:`dynamic`;
* :doc:`rozdziałem wprowadzającym do trasowania <../../book/routing>` of the book;
* :doc:`dokumentacją komponentu trasowania <../../components/routing/introduction>`
  w celu poznania szczegółów implementacyjnych routerów;
* dokumentacja komponentu `Routing`_ Symfony2.

.. _`przy pomocy Composera`: http://getcomposer.org
.. _`symfony-cmf/routing-bundle`: https://packagist.org/packages/symfony-cmf/routing-bundle
.. _`RoutingBundle`: https://github.com/symfony-cmf/RoutingBundle#readme
.. _`PHPCR-ODM`: http://www.doctrine-project.org/projects/phpcr-odm.html
.. _`dokumentacją Symfony2 dla tagów DependencyInjection`: http://symfony.com/doc/2.1/reference/dic_tags.html
.. _`Routing`: http://symfony.com/doc/current/components/routing/introduction.html
