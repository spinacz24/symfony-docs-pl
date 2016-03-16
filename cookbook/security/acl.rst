.. index::
   single: bezpieczeństwo; ACL
   single: autoryzacja; ACL
   single: lista kontroli dostępu (ACL)
   single: ACL

Jak używać list kontroli dostępu (ACL)
======================================

W skomplikowanych aplikacjach, pojawia się często problem, że decyzje o dostępie
nie mogą opierać się tylko na osobie (tokenie) żądającej dostępu, ale również
powinny obejmować obiekt domeny, do którego jest zgłaszany dostęp. W tym miejscu pojawia
się system ACL.

.. sidebar:: Alternatywy dla list ACL

    Używanie list ACL nie jest trywialne i ich stosowanie dla prostszych przypadków
    może być przesadą.
    Jeśli logika uprawnień może być opisana tylko przez napisanie jakiegoś kodu
    (np. aby sprawdzić czy blog jest własnością bieżącego użytkownika), to można
    rozważyć użycie :doc:`wyborców autoryzacji </cookbook/security/voters>`.
    Wyborca autoryzacji jest przekazywany do obiektu będącego przedmiotem głosowania,
    za pomocą którego można dokonywać skomplikowanych decyzji i efektywnie zaimplementować
    własną listę ACL. Wymuszanie autoryzacji (np. część ``isGranted``) będzie
    wygladać podobnie do tego, co można zobaczyć w tym artykule, ale klasa wyborcy
    bedzie obsługiwać w tle tą logikę, zamiast system ACL.

Proszę sobie wyobrazić, że projektujemy system blogu, gdzie użytkownicy mogą
komentować wpisy. Chcemy też, aby użytkownik mógł edytować swoje komentarze,
ale nie komentarze innych użytkowników. Poza tym, chcemy, aby administrator
mógł edytować wszystkie komentarze. W tym scenariuszu, ``Comment`` będzie obiektem
domeny, do którego chcemy ograniczyć dostęp. Można wypróbować kilka sposobów,
aby osiągnąć ten cel w Symfony. Są dwa podstawowe podejścia:

- *Wymuszanie bezpieczeństwa w metodach biznesowych*: Zasadniczo, oznacza to
  utrzymywanie odniesienia wewnątrz każdego obiektu ``Comment`` do wszystkich
  użytkowników, którzy mają dostęp i następnie porównanie tych użytkowników
  z dostarczonym tokenem.
- *Wymuszanie bezpieczeństwa poprzez role*: W podejściu tym, trzeba dodać rolę
  dla każdego obiektu ``Comment``, np. ``ROLE_COMMENT_1``, ``ROLE_COMMENT_2``
  itd.

Oba podejscia są całkowicie poprawne. Jednak, autoryzacja oparta na kodzie logiki
biznesowej nie nadaje się do wielokrotnego stosowania i także utrudnia wykonanie
testów jednostkowych. Poza tym, można napotkać problemy z wydajnością, jeśli wielu
użytkowników będzie miało dostęp do pojedynczego obiektu domeny.

Na szczęście istnieje lepszy sposób, który omówimy teraz.

Wstępna konfiguracja
--------------------

Teraz, zanim będzie można w końcu nabrać akcję, trzeba będzie wykonać jakieś
wstępną konfigurację.
Po pierwsze, trzeba skonfigurować połączenie dla systemu ACL, który ma być używany:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            acl:
                connection: default

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

                <acl connection="default" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', 'acl', array(
            // ...

            'connection' => 'default',
        ));

.. note::

    System ACL wymaga połączenia z Doctrine DBAL (stosowane domyślnie) albo
    z Doctrine MongoDB (stsosowane w `MongoDBAclBundle`_). Jednak nie oznacza to,
    że koniecznie trzeba stosować Doctrine ORM lub ODM dla mapowania obiektów
    domeny. Można stosować jakiegokolwiek mapowania dla swoich obiektów:
    Doctrine ORM, MongoDB ODM, Propel, surowego SQL itd.

Po skonfigurowaniu połączenia można zaimportować strukturę bazy danych.
Wystarczy uruchomić następujące polecenie konsolowe:

.. code-block:: bash

    $ php bin/console init:acl

.. index::
   single: bezpieczeństwo; ACE
   single: autoryzacja; ACE
   single: lista kontroli dostępu (ACL); wpis ACE
   single: ACE

Rozpoczynamy
------------

Rozpocznijmy przykład zaimplementowania listy ACL.

Po utworzeniu ACL, można przyznawać dostęp do obiektów, tworząc wpis kontroli
dostępu (*ang. Access Control Entry - ACE*) w celu ustalenia zależności pomiędzy
encją a użytkownikiem.

Utworzenie listy ACL i dodanie wpisu ACE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: php
   :linenos:

    // src/AppBundle/Controller/BlogController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    use Symfony\Component\Security\Acl\Domain\ObjectIdentity;
    use Symfony\Component\Security\Acl\Domain\UserSecurityIdentity;
    use Symfony\Component\Security\Acl\Permission\MaskBuilder;

    class BlogController extends Controller
    {
        // ...

        public function addCommentAction(Post $post)
        {
            $comment = new Comment();

            // ... setup $form, and submit data

            if ($form->isValid()) {
                $entityManager = $this->getDoctrine()->getManager();
                $entityManager->persist($comment);
                $entityManager->flush();

                // creating the ACL
                $aclProvider = $this->get('security.acl.provider');
                $objectIdentity = ObjectIdentity::fromDomainObject($comment);
                $acl = $aclProvider->createAcl($objectIdentity);

                // retrieving the security identity of the currently logged-in user
                $tokenStorage = $this->get('security.token_storage');
                $user = $tokenStorage->getToken()->getUser();
                $securityIdentity = UserSecurityIdentity::fromAccount($user);

                // grant owner access
                $acl->insertObjectAce($securityIdentity, MaskBuilder::MASK_OWNER);
                $aclProvider->updateAcl($acl);
            }
        }
    }

W tym fragmencie kodu istnieje kilka ważnych decyzji implementacyjnych.
Na razie zajmiemy się tylko dwoma.

Po pierwsze, można zauważyć, że metoda ``->createAcl()`` nie akceptuje bezpośrednio
obiektów domeny, ale tylko implementuje ``ObjectIdentityInterface``.
Ten dodatkowy krok pośredni pozwala na pracę z listami ACL, nawet gdy faktycznie
nie ma żadnego obiektu domeny. Jest to niezwykle pomocne, gdy chce się sprawdzić
uprawnienia dla większej ilości obiektów bez faktycznego przygotowania tych obiektów.

Drugą interesującą częścią jest wywołanie ``->insertObjectAce()``. W tym przykładzie,
udzialmy własnosciowego prawa dostępu do obiektu ``Comment`` użytkownikowi, który
jest aktualnie zalogowany. ``MaskBuilder::MASK_OWNER`` jest wstępnie zdefiniowaną
liczbową maską bitową. Nie przejmuj się, że narzędzie do tworzenia maski ukrywa
większość szczegółów technicznych, ale stosując tą technikę można przechowywać
różne uprawnienia w jednym wierszu bazy danych, co daje znaczny wzrost wydajności.

.. tip::

    Znacząca jest lolejność w jakiej sprawdzane są wpisy ACE. Zgodnie z ogólna
    zasada, na początku powinno sie umieszczać wpisy bardziej szczegółowe.

Sprawdzanie dostępu
~~~~~~~~~~~~~~~~~~~

.. code-block:: php
   :linenos:

    // src/AppBundle/Controller/BlogController.php

    // ...

    class BlogController
    {
        // ...

        public function editCommentAction(Comment $comment)
        {
            $authorizationChecker = $this->get('security.authorization_checker');

            // check for edit access
            if (false === $authorizationChecker->isGranted('EDIT', $comment)) {
                throw new AccessDeniedException();
            }

            // ... retrieve actual comment object, and do your editing here
        }
    }

W tym przykładzie, sprawdzamy, czy użytkownik ma uprawnienia ``EDIT``.
Wewnętrznie Symfony odwzorowuje uprawnienia na różne liczbowe maski bitowe i sprawdza,
czy użytkownik ma któreś z uprawnień.

.. note::

    Mozna zdefiniować 32 podstawowe uprawnienia (w zależności od systemu operacyjnego,
    w PHP liczba ta waha się pomiędzy 30 a 32). Dodatkowo, można zdefiniować
    skumulowane uprawnienia.

Skumulowane uprawnienia
-----------------------

W pierwszym przykładzie, przyznaliźmy użytkownikowi podstawowe uprawnienie ``OWNER``.
Jednocześnie pozwoliliśmy, aby użytkownik mógł wykonywać każdą operację, taką jak
``view``, ``edit`` itd., na obiekcie domeny. Istnieją przypadki w których
potrzeba udzielić tych uprawnień w sposób jawny.

``MaskBuilder`` może zostać użyty do tworzenia masek bitowych przez łączenie
kilku podstawowych uprawnień:

.. code-block:: php

    $builder = new MaskBuilder();
    $builder
        ->add('view')
        ->add('edit')
        ->add('delete')
        ->add('undelete')
    ;
    $mask = $builder->get(); // int(29)

Ta liczbowa maska bitowa może zostać następnie wykorzystana do udzielenia podstawowych
uprawnień dodanych powyżej:

.. code-block:: php
   :linenos:

    $identity = new UserSecurityIdentity('johannes', 'Acme\UserBundle\Entity\User');
    $acl->insertObjectAce($identity, $mask);

Użytkownik może teraz przeglądać, edytować, usuwać i przywracać usunięte obiekty.

.. _`MongoDBAclBundle`: https://github.com/IamPersistent/MongoDBAclBundle
