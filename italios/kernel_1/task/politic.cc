#include <tree>
#include <cstddef>
#include <task.h>
#include <video.h>
#include <rand.h>

namespace {
	const size_t max_biglietti = 5000;
	size_t current_max_ticket = 0;


	struct biglietto {
		pid_t proprietario;
		size_t numero;
		bool operator <(biglietto & altro) {
			return numero < altro.numero;
		}
		bool operator <=(biglietto & altro) {
			return numero <= altro.numero;
		}

		bool operator >(biglietto & altro) {
			return numero > altro.numero;
		}

		bool operator >=(biglietto & altro) {
			return numero >= altro.numero;
		}

		bool operator ==(biglietto & altro) {
			return numero == altro.numero;
		}

		bool operator !=(biglietto & altro) {
			return numero != altro.numero;
		}
	};

	rbtree < biglietto > lotteria;
};

void init_sched_politic()
{
	kout << "Inzializzata la politica dello scheduler" << endl;
	lotteria.init();
}

biglietto_t prendi_biglietti(pid_t pid, size_t num)
{
	biglietto task_ticket;
	task_ticket.proprietario = pid;

	biglietto_t ticket;
	ticket.inizio = current_max_ticket;
	ticket.num = num;
	ticket.proprietario = pid;

	for (size_t i = 0; i < num; i++) {
		task_ticket.numero = current_max_ticket++;
		lotteria.insert(task_ticket);
	}
	return ticket;;
}

void libera_biglietti(biglietto_t ticket)
{
	biglietto task_ticket;
	task_ticket.proprietario = ticket.proprietario;
	for (size_t i = ticket.inizio; i < (ticket.inizio + ticket.num); i++) {
		task_ticket.numero = i;
		lotteria.remove(task_ticket);
	}
}

pid_t find_next_task()
{
	biglietto chiave;
	chiave.numero = casual_n(current_max_ticket);
	if (!lotteria.find(chiave, &chiave))
		return find_next_task();
	return chiave.proprietario;
}
