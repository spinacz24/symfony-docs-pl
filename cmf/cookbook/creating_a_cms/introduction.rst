.. index::
    single: poradnik, tworzenie CMS, RoutingAuto, PHPCR-ODM
    single: MenuBundle, SonataAdmin, SonataDoctrineAdminBundle

Tworzenie prostego CMS przy użyciu RoutingAutoBundle
====================================================

Ta seria artykułów pokazuje jak można utworzyć od podstaw prosty system CMS,
wykorzystując następujące pakiety:

* :doc:`../../bundles/routing_auto/index`;
* :doc:`../../bundles/phpcr_odm/index`;
* :doc:`../../bundles/menu/index`;
* SonataDoctrinePHPCRAdminBundle_.

Zakłada się, że masz:

* praktyczną wiedzę o frameworku Symfony;
* podstawową wiedzę o PHPCR-ODM.

Nasz CMS bedzie miał dwa rodzaje treści:

* **Strony**: treści HTML dostępna pod adresami URL, na przykład ``/page/home``, ``/page/about`` itd.;
* **Wpisy**: wpisy blogowe dostępne jako ``/blog/2012/10/23/my-blog-post``.

Automatyczna integracja tras będzie automatycznie tworzyć i aktualizować trasy
(skuteczne adresy URL przy których można uzyskać dostęp do treści) dla dokumentów
treści stron i wpisów. Ponadto każdy dokument treści podwoi się jako element menu.

.. image:: ../../_images/cookbook/basic-cms-intro-sketch.png

.. note::

    Istnieje pakiet o nazwie :doc:`../../bundles/simple_cms/index`, który dostarcza
    podobnego rozwiązania do proponowanego w tym poradniku. Łączy on trasę, menu
    i treść w pojedynczy dokument i używa własnego routera. Podejście prezentowane
    w tym poradniku łączy w jeden dokument tylko menu i treść. Trasy będą zarządzane
    automatyczne i wykorzystany będzie natywny ``DynamicRouter`` CMF.

.. _SonataDoctrinePHPCRAdminBundle: https://github.com/sonata-project/SonataDoctrinePhpcrAdminBundle
