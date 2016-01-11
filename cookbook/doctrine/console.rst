.. index::
   single: Doctrine; polecenia konsolowe ORM
   single: CLI; Doctrine ORM

Polecenia konsolowe
-------------------

Po włączeniu ORM Doctrine2 dostępne jest kilka poleceń konsolowych w przestrzeni
nazewniczej ``doctrine``. W celu wyświetlenia dostępnych poleceń trzeba użyć
polecenia ``list``::

.. code-block:: bash

    $ php app/console list doctrine

Można znaleźć więcej informacji o każdym tym poleceniu (lub każdym poleceniu Symfony)
przez uruchomienie polecenia ``help``. Na przykład, aby pobrać szczegóły o poleceniu
``doctrine:database:create``, uruchom:

.. code-block:: bash

    $ php app/console help doctrine:database:create

Pewne zadania godne uwagi, to:

* ``doctrine:ensure-production-settings`` - sprawdza, czy bieżące środowisko
  zostało skonfigurowane jako produkcyjne. Polecenie to powinno się zawsze
  uruchamiać w środowisku ``prod``:

  .. code-block:: bash

      $ php app/console doctrine:ensure-production-settings --env=prod

* ``doctrine:mapping:import`` - powoduje, aby Doctrine dokonało introspekcji
  istniejącej bazy danych i utworzyło informacje mapowania. Więcej informacji
  na ten temat znajduje się w artykule :doc:`/cookbook/doctrine/reverse_engineering`.

* ``doctrine:mapping:info`` - powiadamia o wszystkim dotyczącym encji, które obsługuje
  Doctrine oraz o tym, czy wystęþuja jakieś podstawowe błędy w mapowaniu.

* ``doctrine:query:dql`` i ``doctrine:query:sql`` - umożliwiaja wykonanie
  zapytań DQL lub SQL bezposrednio z linii poleceń.
