#include <linux/kernel.h>
#include <asm-generic/errno.h>
#include <asm/current.h>
#include <linux/sched.h>

asmlinkage long sys_hello(void) {
	printk("Hello, World!\n");
	return 0;
}

asmlinkage long sys_set_weight(int weight) {
	if (weight < 0) {
		return -1*EINVAL;
	}

	current->weight = weight;
	
	return 0;
}

asmlinkage long sys_get_weight(void) {
	return current->weight;
}

int get_children_sum_aux(struct task_struct* task){
	int sum =  task->weight;
	struct task_struct* child;
	struct list_head* list;
	list_for_each(list, &task->children) {
		child = list_entry(list, struct task_struct, sibling);
		sum += get_children_sum_aux(child);
	}
	
	return sum;
}

asmlinkage long sys_get_children_sum(void) {
	int sum = get_children_sum_aux(current) - current->weight;
	if (sum==0) {
		return -1*ECHILD;
	}
	
	return sum;
}

asmlinkage long sys_get_heaviest_ancestor(void) {
	int max = current->weight;
	pid_t max_pid = current->pid;
	struct task_struct* parent1;
	if(current->pid == 1){
		return 1;
	}
	parent1 = current->parent;
	while (1){
		if (parent1->weight > max){
			max = parent1->weight;
			max_pid = parent1->pid;
		}
		
		if (parent1->pid == 1){
			return max_pid;
		}
		
		parent1 = parent1->parent;
	}
	
	return max_pid;
}
