.. highlight:: php
   :linenothreshold: 2

.. index::
   single: polecenia konsoli; generate:doctrine:entity
   single: generowanie kawałka encji Doctrine
   
Generowanie nowego kawałka encji Doctrine
-----------------------------------------

Stosowanie
~~~~~~~~~~

Polecenie **generate:doctrine:entity** generuje nowy kawałek encji Doctrine, w tym
definicję odwzorowania i właściwości klasy, metody akcesorów pobierających i ustawiających.

Domyślnie polecenie jest uruchamiane w trybie interaktywnym i zadaje pytania w celu
ustalenia nazwy pakietu, lokalizacji, formatu konfiguracji i domyślnej struktury:

.. code-block:: bash
   
   $ php app/console generate:doctrine:entity
   
Polecenie to można uruchomić w trybie zwykłym (nieinteraktywnym) używając opcję
``--no-interaction``, nie zapominając o przekazaniu wszystkich niezbędnych opcji:

.. code-block:: bash
   
   $ php app/console generate:doctrine:entity --no-interaction --entity=AcmeBlogBundle:Post --fields="title:string(100) body:text" --format=xml

Dostępne opcje
~~~~~~~~~~~~~~

*  ``--entity``: Nazwa encji, podana w notacji skrótowej, zawierająca nazwę pakietu
   w którym umieszczona jest encja i nazwę encji. Na przykład ``AcmeBlogBundle:Post``:
   
   .. code-block:: bash
      
      $ php app/console generate:doctrine:entity --entity=AcmeBlogBundle:Post
      
*  ``--fields``: Lista pól generowanych w klasie encji:

   .. code-block:: bash
      
      $ php app/console generate:doctrine:entity --fields="title:string(100) body:text"
      
*  ``--format``: (**annotation**) [wartości: yml, xml, php lub annotation]
   Opcja określa format jaki ma być zastosowany przy generowaniu plików konfiguracyjnych,
   takich jak trasowanie. Domyślnie polecenie wykorzystuje format adnotacji:
   
   .. code-block:: bash
      
      $ php app/console generate:doctrine:entity --format=annotation
      
*  ``--with-repository``: Opcja ta informuje, czy generować związaną klasę EntityRepository
   biblioteki Doctrine:
   
   .. code-block:: bash
      
      $ php app/console generate:doctrine:entity --with-repository
      
   