/* File: cmd3.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 *
 * UnAngband (c) 2001 Andrew Doull. Modifications to the Angband 2.9.1
 * source code are released under the Gnu Public License. See www.fsf.org
 * for current GPL license details. Addition permission granted to
 * incorporate modifications in all Angband variants as defined in the
 * Angband variants FAQ. See rec.games.roguelike.angband for FAQ.
 */

#include "angband.h"






/*
 * Display inventory
 */
void do_cmd_inven(void)
{
	/* Hack -- Start in "inventory" mode */
	p_ptr->command_wrk = (USE_INVEN);

	/* Save screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the inventory */
	show_inven();

	/* Hack -- hide empty slots */
	item_tester_full = FALSE;

	/* Prompt for a command */
	prt("(Inventory) Command: ", 0, 0);

	/* Hack -- Get a new command */
	p_ptr->command_new = inkey();

	/* Load screen */
	screen_load();


	/* Hack -- Process "Escape" */
	if (p_ptr->command_new == ESCAPE)
	{
		/* Reset stuff */
		p_ptr->command_new = 0;
	}

	/* Hack -- Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		p_ptr->command_see = TRUE;
	}
}


/*
 * Display equipment
 */
void do_cmd_equip(void)
{
	/* Hack -- Start in "equipment" mode */
	p_ptr->command_wrk = (USE_EQUIP);

	/* Save screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the equipment */
	show_equip();

	/* Hack -- undo the hack above */
	item_tester_full = FALSE;

	/* Prompt for a command */
	prt("(Equipment) Command: ", 0, 0);

	/* Hack -- Get a new command */
	p_ptr->command_new = inkey();

	/* Load screen */
	screen_load();


	/* Hack -- Process "Escape" */
	if (p_ptr->command_new == ESCAPE)
	{
		/* Reset stuff */
		p_ptr->command_new = 0;
	}

	/* Hack -- Process normal keys */
	else
	{
		/* Enter "display" mode */
		p_ptr->command_see = TRUE;
	}
}


/*
 * The "wearable" tester
 */
static bool item_tester_hook_wear(const object_type *o_ptr)
{
	/* Check for a usable slot */
	if (wield_slot(o_ptr) >= INVEN_WIELD) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


/*
 * Wield or wear a single item from the pack or floor
 * Now supports wearing multiple rings.
 */
void do_cmd_wield(void)
{
	int item, slot;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	cptr act;

	cptr q, s;

	char o_name[80];

	u32b f1, f2, f3;
	u32b n1, n2, n3;
	u32b k1, k2, k3;

	/* Hack -- Allow items to be swapped in/out of belt */
	bool swap = FALSE;

	/* Hack -- Allow multiple rings to be wielded */
	int amt=1;
	int rings = 0;

	/* Restrict the choices */
	item_tester_hook = item_tester_hook_wear;

	/* Get an item */
	q = "Wear/Wield which item? ";
	s = "You have nothing you can wear or wield.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR | USE_FEATG))) return;

	/* Get the feature */
	if (item >= INVEN_TOTAL+1)
	{
		object_type object_type_body;

		o_ptr = &object_type_body;

		if (!make_feat(o_ptr, cave_feat[p_ptr->py][p_ptr->px])) return;
	}
	/* Get the item (in the pack) */
	else if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Check the slot */
	slot = wield_slot(o_ptr);

	/* Hack -- slot not allowed */
	if (slot < 0) return;

	/* Hack -- wield from belt to equip */
	if ((variant_belt_slot) && (item == INVEN_BELT))
	{
		/* Swap items? */
		if (inventory[slot].k_idx) swap = TRUE;
	}
	/* Hack -- wield from equip to belt */
	else if ((variant_belt_slot) && (item >= INVEN_WIELD) && (item < INVEN_TOTAL+1))
	{
		/* Pick the slot */
		slot = INVEN_BELT;

		/* Swap items? */
		if ((inventory[INVEN_BELT].k_idx) &&
		    (wield_slot(&inventory[INVEN_BELT]) == item)) swap = TRUE;
	}

	/* Hack -- don't dual wield */
	if ((slot == INVEN_ARM) && (o_ptr->tval != TV_SHIELD))
	{
		if (!get_check("Wield it in your off-hand? "))
		{
			slot = INVEN_WIELD;
		}
	}

	/* Hack -- wield amount */
	else if ((variant_fast_equip) && (slot == INVEN_WIELD))
	{

		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;

		/* Cancel swap items */
		if (amt < o_ptr->number) swap = FALSE;
	}
	/* Hack -- belt amount */
	else if ((variant_belt_slot) && (slot == INVEN_BELT))
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;

		/* Cancel swap items */
		if (amt < o_ptr->number) swap = FALSE;
	}
	/* Hack -- multiple rings */
	else if ((variant_many_rings) && (o_ptr->tval == TV_RING))
	{
		i_ptr = &inventory[slot];

		/* Wear multiple rings */
		if (object_similar(o_ptr,i_ptr)) rings = 5-i_ptr->number;

		/* Get a quantity - take off existing rings */
		if (!rings) amt = get_quantity(NULL, (o_ptr->number < 5) ? o_ptr->number : 5);

		/* Get a quantity - add to existing rings */
		else amt = get_quantity(NULL, (o_ptr->number < rings) ? o_ptr->number : rings);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Source and destination identical */
	if (item == slot) return;

	/* Prevent wielding into a cursed slot */
	if (cursed_p(&inventory[slot]) && (slot < INVEN_BELT))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], FALSE, 0);

		/* Message */
		msg_format("The %s you are %s appears to be cursed.",
		   o_name, describe_use(slot));

		/* Cancel the command */
		return;
	}
	/* Prevent wielding from cursed slot */
	else if (cursed_p(&inventory[item]) && (item >= INVEN_WIELD) && (item != INVEN_BELT))
	{
		/* Describe it */
		object_desc(o_name, &inventory[item], FALSE, 0);

		/* Message */
		msg_format("The %s you are %s appears to be cursed.",
		   o_name, describe_use(item));

		/* Cancel the command */
		return;
	}

	/* Take a (partial) turn */
	if ((variant_fast_floor) && (item < 0)) p_ptr->energy_use = 50;
	else if ((variant_fast_equip) && (item >= INVEN_WIELD)) p_ptr->energy_use = 50;
	else p_ptr->energy_use = 100;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain local object */
	object_copy(i_ptr, o_ptr);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Reset stackc */
	i_ptr->stackc = 0;

	/* Sometimes use lower stack object */
	if (!object_known_p(o_ptr) && (rand_int(o_ptr->number)< o_ptr->stackc))
	{
		if (amt >= o_ptr->stackc)
		{
			i_ptr->stackc = o_ptr->stackc;

			o_ptr->stackc = 0;
		}
		else
		{
			if (i_ptr->pval) i_ptr->pval--;
			if (i_ptr->timeout) i_ptr->timeout = 0;

			o_ptr->stackc -= amt;
		}
	}

	/* Hack -- clear may flags to avoid forgetting them */
	/* This also prevents 2 identical stacks of the same item */
	/* preventing identification of the wielded one */
	drop_may_flags(o_ptr);

	/* Decrease the feature */
	if (item >= INVEN_TOTAL+1)
	{
		cave_alter_feat(p_ptr->py,p_ptr->px,FS_GET_FEAT);
	}
	/* Decrease the item (from the pack) */
	else if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_optimize(item);
	}
	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_optimize(0 - item);
	}

	/* Get the wield slot */
	o_ptr = &inventory[slot];

	/* Swap existing item. Note paranoia check (item >= INVEN_WIELD). */
	if ((o_ptr->k_idx) && (swap) && (item >= INVEN_WIELD))
	{
		/* Get the old slot */
		object_type *j_ptr = &inventory[item];

		/* Swap the items */
		object_copy(j_ptr, o_ptr);

	}
	/* Drop existing item */
	else if ((o_ptr->k_idx) && (p_ptr->energy_use == 50))
	{
		/* Take off existing item */
		(void)inven_drop(slot, 255);
	}
	/* Take off existing item */
	else if ((o_ptr->k_idx) && (!rings))
	{
		/* Take off existing item */
		(void)inven_takeoff(slot, 255);
	}

	/* Wear the new rings */
	if (rings) object_absorb(o_ptr, i_ptr);

	/* Wear the new stuxff */
	else object_copy(o_ptr, i_ptr);

	/* Increase the weight */
	p_ptr->total_weight += i_ptr->weight * amt;

	/* Increment the equip counter by hand */
	p_ptr->equip_cnt++;

	/* Where is the item now */
	if (((slot == INVEN_WIELD) && (o_ptr->number > 1)) || (slot == INVEN_BELT))
	{
		act = "You are carrying";
	}
	else if ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM)
		|| (o_ptr->tval == TV_HAFTED) || (o_ptr->tval == TV_STAFF) ||
		(o_ptr->tval == TV_DIGGING))
	{
		act = "You are wielding";
		if (slot == INVEN_ARM) act = "You are wielding off-handed";
	}
	else if (slot == INVEN_WIELD)
	{
		act = "You are using";
	}
	else if (o_ptr->tval == TV_INSTRUMENT)
	{
		act = "You are playing music with";
	}
	else if (slot == INVEN_BOW)
	{
		act = "You are shooting with";
	}
	else if (slot == INVEN_LITE)
	{
		act = "Your light source is";
	}
	else
	{
		act = "You are wearing";
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format("%s %s (%c).", act, o_name, index_to_label(slot));

	/* Cursed! */
	if (cursed_p(o_ptr))
	{
		/* Warn the player */
		msg_print("Oops! It feels deathly cold!");

		/* Remove special inscription, if any */
		if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

		/* Sense the object if allowed */
		if (o_ptr->discount == 0) o_ptr->discount = INSCRIP_CURSED;

		/* The object has been "sensed" */
		o_ptr->ident |= (IDENT_SENSE);

	}

	/* Get known flags */
	k1 = o_ptr->can_flags1;
	k2 = o_ptr->can_flags2;
	k3 = o_ptr->can_flags3;

	/* Some flags are instantly known */
	object_flags(o_ptr,&f1,&f2,&f3);

	/* Hack -- the following are obvious from the displayed combat statistics */
	if (f1 & (TR1_BLOWS)) object_can_flags(o_ptr,TR1_BLOWS,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_BLOWS,0x0L,0x0L);

	if (f1 & (TR1_SHOTS)) object_can_flags(o_ptr,TR1_SHOTS,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_SHOTS,0x0L,0x0L);

	/* Hack -- the following are obvious from the displayed combat statistics */
	if (f1 & (TR1_SPEED)) object_can_flags(o_ptr,TR1_SPEED,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_SPEED,0x0L,0x0L);

	/* Hack -- the following are obvious from the displayed stats */
	if (f1 & (TR1_STR)) object_can_flags(o_ptr,TR1_STR,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_STR,0x0L,0x0L);

	if (f1 & (TR1_INT)) object_can_flags(o_ptr,TR1_INT,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_INT,0x0L,0x0L);

	if (f1 & (TR1_WIS)) object_can_flags(o_ptr,TR1_WIS,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_WIS,0x0L,0x0L);

	if (f1 & (TR1_DEX)) object_can_flags(o_ptr,TR1_DEX,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_DEX,0x0L,0x0L);

	if (f1 & (TR1_CON)) object_can_flags(o_ptr,TR1_CON,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_CON,0x0L,0x0L);

	if (f1 & (TR1_CHR)) object_can_flags(o_ptr,TR1_CHR,0x0L,0x0L);
	else object_not_flags(o_ptr,TR1_CHR,0x0L,0x0L);

#ifndef ALLOW_OBJECT_INFO_MORE
	/* Hack --- we do these here, because they are too computationally expensive in the 'right' place */
	if (f1 & (TR1_INFRA))
	{
		object_can_flags(o_ptr,TR1_INFRA,0x0L,0x0L);
	}
	else object_not_flags(o_ptr,TR1_INFRA,0x0L,0x0L);

	if (f3 & (TR3_LITE))
	{
		object_can_flags(o_ptr,0x0L,0x0L,TR3_LITE);
	}
	else object_not_flags(o_ptr,0x0L,0x0L,TR3_LITE);

	/* Hack --- also computationally expensive. But note we notice these only if we don't already */
	/* have these abilities */
	if ((f3 & (TR3_TELEPATHY)) && !(p_ptr->telepathy)) object_can_flags(o_ptr,0x0L,0x0L,TR3_TELEPATHY);
	else object_not_flags(o_ptr,0x0L,0x0L,TR3_TELEPATHY);

	if ((f3 & (TR3_SEE_INVIS)) && (!p_ptr->see_inv) && (!p_ptr->tim_invis)) object_can_flags(o_ptr,0x0L,0x0L,TR3_SEE_INVIS);
	else object_not_flags(o_ptr,0x0L,0x0L,TR3_SEE_INVIS);
#endif

	/* Hack --- the following are either obvious or (relatively) unimportant */
	if (f3 & (TR3_BLESSED))
	{
		object_can_flags(o_ptr,0x0L,0x0L,TR3_BLESSED);
	}
	else object_not_flags(o_ptr,0x0L,0x0L,TR3_BLESSED);

	if (f3 & (TR3_LIGHT_CURSE)) object_can_flags(o_ptr,0x0L,0x0L,TR3_LIGHT_CURSE);
	else object_not_flags(o_ptr,0x0L,0x0L,TR3_LIGHT_CURSE);

	/* Check for new flags */
	n1 = o_ptr->can_flags1 & ~(k1);
	n2 = o_ptr->can_flags1 & ~(k2);
	n3 = o_ptr->can_flags1 & ~(k3);

	if (n1 || n2 || n3) update_slot_flags(INVEN_WIELD, n1, n2, n3);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
}



/*
 * Take off an item
 */
void do_cmd_takeoff(void)
{
	int item;

	object_type *o_ptr;

	cptr q, s;


	/* Get an item */
	q = "Take off which item? ";
	s = "You are not wearing anything to take off.";
	if (!get_item(&item, q, s, (USE_EQUIP))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Item is cursed */
	if (cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}


	/* Take a partial turn */
	p_ptr->energy_use = 50;

	/* Take off the item */
	(void)inven_takeoff(item, 255);
}


/*
 * Drop an item
 */
void do_cmd_drop(void)
{
	int item, amt;

	object_type *o_ptr;

	cptr q, s;


	/* Get an item */
	q = "Drop which item? ";
	s = "You have nothing to drop.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Get a quantity */
	amt = get_quantity(NULL, o_ptr->number);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}

	/* Take a partial turn */
	p_ptr->energy_use = 50;

	/* Drop (some of) the item */
	inven_drop(item, amt);
}



/*
 * Destroy an item
 */
void do_cmd_destroy(void)
{
	int item, amt;
	int old_number;

	object_type *o_ptr;

	char o_name[80];

	char out_val[160];

	cptr q, s;

	/* Get an item */
	q = "Destroy which item? ";
	s = "You have nothing to destroy.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Get a quantity */
	amt = get_quantity(NULL, o_ptr->number);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, TRUE, 3);
	o_ptr->number = old_number;

	/* Verify destruction */
	if (verify_destroy)
	{
		sprintf(out_val, "Really destroy %s? ", o_name);
		if (!get_check(out_val)) return;
	}

	/* Take a (partial) turn */
	if ((variant_fast_floor) && (item < 0)) p_ptr->energy_use = 50;
	else if ((variant_fast_equip) && (item >= INVEN_WIELD)) p_ptr->energy_use = 50;
	else p_ptr->energy_use = 100;

	/* Containers release contents */
	if ((o_ptr->tval == TV_HOLD) && (o_ptr->name3 > 0))
	{
		if (animate_object(item)) return;

		/* Message */
		msg_format("You cannot destroy %s.", o_name);

		return;
	}

	/* Artifacts cannot be destroyed */
	if (artifact_p(o_ptr))
	{

		/* Message */
		msg_format("You cannot destroy %s.", o_name);

		/* Sense the object if allowed, don't sense ID'ed stuff */
		if ((o_ptr->discount == 0)
		&& !(o_ptr->ident & (IDENT_SENSE))
		 && !(object_known_p(o_ptr)))
		{
			o_ptr->discount = INSCRIP_UNBREAKABLE;

			/* The object has been "sensed" */
			o_ptr->ident |= (IDENT_SENSE);

			/* Combine the pack */
			p_ptr->notice |= (PN_COMBINE);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN | PW_EQUIP);

		}

		/* Done */
		return;

	}

	/* Message */
	msg_format("You destroy %s.", o_name);

	/* Sometimes use lower stack object */
	if (!object_known_p(o_ptr) && (rand_int(o_ptr->number)< o_ptr->stackc))
	{
		if (amt >= o_ptr->stackc)
		{
			o_ptr->stackc = 0;
		}
		else
		{
			o_ptr->stackc -= amt;
		}
	}

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

}


/*
 * Observe an item which has been *identify*-ed
 */
void do_cmd_observe(void)
{
	int item;

	object_type *o_ptr;

	char o_name[80];

	cptr q, s;


	/* Get an item */
	q = "Examine which item? ";
	s = "You have nothing to examine.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;


	/* Get the feature */
	if (item >= INVEN_TOTAL+1)
	{
		object_type object_type_body;

		o_ptr = &object_type_body;

		if (!make_feat(o_ptr, cave_feat[p_ptr->py][p_ptr->px])) return;
	}
	/* Get the item (in the pack) */
	else if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Describe */
	msg_format("Examining %s...", o_name);

	msg_print("");

	/* Describe */
	screen_object(o_ptr, TRUE);
}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
void do_cmd_uninscribe(void)
{
	int item;

	object_type *o_ptr;

	cptr q, s;

	int i;

	/* Get an item */
	q = "Un-inscribe which item? ";
	s = "You have nothing to un-inscribe.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Nothing to remove */
	if (!o_ptr->note)
	{
		msg_print("That item had no inscription to remove.");
		return;
	}

	/* Message */
	msg_print("Inscription removed.");

	/* Remove the incription */
	o_ptr->note = 0;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Prompt to always inscribe? */
	if (!easy_autos) return;

	/* Do we inscribe all these ego items? */
	if (object_known_p(o_ptr) && (o_ptr->name2) && (e_info[o_ptr->name2].note))
	{
		e_info[o_ptr->name1].note = 0;

		/* Process objects */
		for (i = 1; i < o_max; i++)
		{
			/* Get the object */
			object_type *i_ptr = &o_list[i];

			/* Skip dead objects */
			if (!i_ptr->k_idx) continue;

			/* Not matching ego item */
			if (i_ptr->name2 != o_ptr->name2) continue;

			/* Auto-inscribe */
			if (object_known_p(i_ptr) || cheat_auto) i_ptr->note = 0;

		}

	}
	/* Do we inscribe all these object kinds? */
	else if (object_aware_p(o_ptr) && (k_info[o_ptr->k_idx].note))
	{
		k_info[o_ptr->k_idx].note = 0;

		/* Process objects */
		for (i = 1; i < o_max; i++)
		{
			/* Get the object */
			object_type *i_ptr = &o_list[i];

			/* Skip dead objects */
			if (!i_ptr->k_idx) continue;

			/* Not matching ego item */
			if (i_ptr->k_idx != o_ptr->k_idx) continue;

			/* Auto-inscribe */
			if (object_known_p(i_ptr) || cheat_auto) i_ptr->note = 0;

		}
	}
}


/*
 * Inscribe an object with a comment
 */
void do_cmd_inscribe(void)
{
	int item;

	object_type *o_ptr;

	char o_name[80];

	char tmp[80];

	cptr q, s;

	int i;

	/* Get an item */
	q = "Inscribe which item? ";
	s = "You have nothing to inscribe.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Describe the activity */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format("Inscribing %s.", o_name);
	msg_print(NULL);

	/* Start with nothing */
	strcpy(tmp, "");

	/* Use old inscription */
	if (o_ptr->note)
	{
		/* Start with the old inscription */
		strnfmt(tmp, 80, "%s", quark_str(o_ptr->note));
	}

	/* Get a new inscription (possibly empty) */
	if (get_string("Inscription: ", tmp, 80))
	{
		/* Save the inscription */
		o_ptr->note = quark_add(tmp);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}

	/* Inscribe */
	if ((item < 0) && (auto_pickup_ignore(o_ptr))) o_ptr->marked = FALSE;

	/* Prompt to always inscribe? */
	if (!easy_autos) return;

	/* Do we inscribe all these ego items? */
	if (object_known_p(o_ptr) && (o_ptr->name2))
	{
		e_info[o_ptr->name1].note = o_ptr->note;

		/* Process objects */
		for (i = 1; i < o_max; i++)
		{
			/* Get the object */
			object_type *i_ptr = &o_list[i];

			/* Skip dead objects */
			if (!i_ptr->k_idx) continue;

			/* Not matching ego item */
			if (i_ptr->name2 != o_ptr->name2) continue;

			/* Already has inscription */
			if (i_ptr->note) continue;

			/* Auto-inscribe */
			if (object_known_p(i_ptr) || cheat_auto) i_ptr->note = e_info[o_ptr->name2].note;

			/* Ignore */
			if (auto_pickup_ignore(o_ptr)) o_ptr->marked = FALSE;

		}

	}
	/* Do we inscribe all these object kinds? */
	else if (object_aware_p(o_ptr))
	{
		k_info[o_ptr->k_idx].note = o_ptr->note;

		/* Process objects */
		for (i = 1; i < o_max; i++)
		{
			/* Get the object */
			object_type *i_ptr = &o_list[i];

			/* Skip dead objects */
			if (!i_ptr->k_idx) continue;

			/* Not matching ego item */
			if (i_ptr->k_idx != o_ptr->k_idx) continue;

			/* Already has inscription */
			if (i_ptr->note) continue;

			/* Auto-inscribe */
			if (object_known_p(i_ptr) || cheat_auto) i_ptr->note = o_ptr->note;

			/* Ignore */
			if (auto_pickup_ignore(o_ptr)) o_ptr->marked = FALSE;

		}

	}
}

/*
 * An "item_tester_hook" for refilling lanterns
 */
static bool item_tester_refill_lantern(const object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if ((o_ptr->tval == TV_FLASK) && (o_ptr->sval == SV_FLASK_OIL)) return (TRUE);

	/* Non-empty lanterns are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN) &&
	    (o_ptr->pval > 0))
	{
		return (TRUE);
	}

	/* Assume not okay */
	return (FALSE);
}


/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(const object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_TORCH)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}

/*
 * An "item_tester_hook" for empty flasks
 */
static bool item_tester_empty_flask_or_lite(const object_type *o_ptr)
{
	/* Empty flasks are okay */
	if ((o_ptr->tval == TV_FLASK) && (o_ptr->sval == SV_FLASK_EMPTY)) return (TRUE);

	/* Empty bottles are okay */
	if ((o_ptr->tval == TV_HOLD) && (o_ptr->sval == SV_HOLD_BOTTLE) && !(o_ptr->name3)) return (TRUE);

	/* Lanterns are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN))
	{
		return (TRUE);
	}

	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_TORCH)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * An "item_tester_hook" for refilling flasks
 */
static bool item_tester_refill_flask(const object_type *o_ptr)
{
	/* Flasks are okay */
	if ((o_ptr->tval == TV_FLASK) && (o_ptr->sval != SV_FLASK_EMPTY)) return (TRUE);

	/* Potions are okay */
	if (o_ptr->tval == TV_POTION)
	{
		return (TRUE);
	}

	/* Assume not okay */
	return (FALSE);
}


/*
 * Fill a flask
 */
void do_cmd_refill(void)
{
	int item, item2;

	object_type *o_ptr;
	object_type *i_ptr;
	object_type *j_ptr;

object_type object_type_body;

	cptr q, s;

bool unstack = FALSE;



	/* Restrict the choices */
	item_tester_hook = item_tester_empty_flask_or_lite;

	/* Get an item */
	q = "Fill/fuel which item? ";
	s = "You have nothing to fill or fuel.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_LANTERN))
	{
		/* Restrict the choices */
		item_tester_hook = item_tester_refill_lantern;

		/* Get an item */
		q = "Refill with which source of oil? ";
		s = "You have no sources of oil.";

		if (!get_item(&item2, q, s, (USE_INVEN | USE_FLOOR | USE_FEATU))) return;
	}
	else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
	{
		/* Restrict the choices */
		item_tester_hook = item_tester_refill_torch;

		/* Get an item */
		q = "Refuel with which torch? ";
		s = "You have no extra torches.";

		if (!get_item(&item2, q, s, (USE_INVEN | USE_FLOOR | USE_FEATG))) return;
	}

	else
	{
		/* Restrict the choices */
		item_tester_hook = item_tester_refill_flask;

		/* Get an item to fill */
		q = "Fill from where? ";
		s = "You have nothing to fill it with.";

		if (!get_item(&item2, q, s, (USE_INVEN | USE_FLOOR | USE_FEATU))) return;
	}

	/* Can't fuel self */
	if (item == item2) return;

	/* Get the feature */
	if (item2 >= INVEN_TOTAL+1)
	{
		object_type object_type_body;

		j_ptr = &object_type_body;

		if (!make_feat(j_ptr, cave_feat[p_ptr->py][p_ptr->px])) return;
	}

	/* Get the item (in the pack) */
	else if (item2 >= 0)
	{
		j_ptr = &inventory[item2];
	}

	/* Get the item (on the floor) */
	else
	{
		j_ptr = &o_list[0 - item2];
	}

	/* Take a partial turn */
	p_ptr->energy_use = 50;

	if (o_ptr->number > 1)
	{
		/* Get local object */
		i_ptr = &object_type_body;

		/* Obtain a local object */
		object_copy(i_ptr, o_ptr);

		/* Modify quantity */
		i_ptr->number = 1;

		/* Reset stack counter */
		i_ptr->stackc = 0;

		/* Unstack the used item */
		o_ptr->number--;

		o_ptr = i_ptr;

		unstack = TRUE;
	}

	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_LANTERN))
	{

		/* Message */
		if (unstack) msg_print("You unstack your lantern.");

		/* Refuel */
		o_ptr->pval += j_ptr->pval;

		/* Message */
		msg_print("You fuel the lamp.");

		/* Comment */
		if (o_ptr->pval >= FUEL_LAMP)
		{
			o_ptr->pval = FUEL_LAMP;
			msg_print("The lamp is full.");
		}
	}
	else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
	{

		/* Message */
		if (unstack) msg_print("You unstack your torch.");

		/* Refuel */
		o_ptr->pval += j_ptr->pval + 5;

		/* Message */
		msg_print("You combine the torches.");

		/* Over-fuel message */
		if (o_ptr->pval >= FUEL_TORCH)
		{
			o_ptr->pval = FUEL_TORCH;
			msg_print("Your torch is fully fueled.");
		}

		/* Refuel message */
		else if (item == INVEN_LITE)
		{
			msg_print("Your torch glows more brightly.");
		}
	}
	else
	{
		if (unstack)
		{
			/* We are done */
		}
		else if (item >= INVEN_TOTAL+1)
		{
			cave_alter_feat(p_ptr->py,p_ptr->px,FS_USE_FEAT);
		}

		/* Decrease the item (in the pack) */
		else if ((item >= 0) && (o_ptr->tval != TV_LITE))
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
		/* Decrease the item (from the floor) */
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}

		/* Adjust the weight and carry */
		item = inven_carry(j_ptr);

	}

	if (unstack)
	{
		/* Adjust the weight and carry */
		p_ptr->total_weight -= o_ptr->weight;
		item = inven_carry(o_ptr);
	}

	/* Decrease the feature */
	if ((item2 >= INVEN_TOTAL+1) && (o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
	{
		cave_alter_feat(p_ptr->py,p_ptr->px,FS_GET_FEAT);
	}
	else if (item2 >= INVEN_TOTAL+1)
	{
		cave_alter_feat(p_ptr->py,p_ptr->px,FS_USE_FEAT);
	}
	/* Use fuel from a lantern */
	else if ((j_ptr->tval == TV_LITE) && (j_ptr->sval == SV_LITE_LANTERN))
	{
		if (j_ptr->number > 1)
		{
			/* Get local object */
			i_ptr = &object_type_body;

			/* Obtain a local object */
			object_copy(i_ptr, j_ptr);

			/* Modify quantity */
			i_ptr->number = 1;

			/* Reset stack counter */
			i_ptr->stackc = 0;

			/* Reset the pval */
			i_ptr->pval = 0;

			/* Unstack the used item */
			j_ptr->number--;

			/* Adjust the weight and carry */
			p_ptr->total_weight -= i_ptr->weight;
			item = inven_carry(i_ptr);

			/* Message */
			msg_print("You unstack your lantern.");
		}
		else
		{
			/* Use up last of fuel */
			j_ptr->pval = 0;
		}
	}
	/* Decrease the item (in the pack) */
	else if (item2 >= 0)
	{
		inven_item_increase(item2, -1);
		inven_item_describe(item2);
		inven_item_optimize(item2);
	}
	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item2, -1);
		floor_item_describe(0 - item2);
		floor_item_optimize(0 - item2);
	}


	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Lite if necessary */
	if ((item == INVEN_LITE) || (item2 == INVEN_LITE)) p_ptr->update |= (PU_TORCH);

}



/*
 * Target command
 */
void do_cmd_target(void)
{
	/* Target set */
	if (target_set_interactive(TARGET_KILL))
	{
		msg_print("Target Selected.");
	}

	/* Target aborted */
	else
	{
		msg_print("Target Aborted.");
	}
}



/*
 * Look command
 */
void do_cmd_look(void)
{
	/* Look around */
	if (target_set_interactive(TARGET_LOOK))
	{
		msg_print("Target Selected.");
	}
}



/*
 * Allow the player to examine other sectors on the map
 */
void do_cmd_locate(void)
{
	int dir, y1, x1, y2, x2;

	char tmp_val[80];

	char out_val[160];


	/* Start at current panel */
	y2 = y1 = p_ptr->wy;
	x2 = x1 = p_ptr->wx;

	/* Show panels until done */
	while (1)
	{
		/* Describe the location */
		if ((y2 == y1) && (x2 == x1))
		{
			tmp_val[0] = '\0';
		}
		else
		{
			sprintf(tmp_val, "%s%s of",
			((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
			((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
		}

		/* Prepare to ask which way to look */
		sprintf(out_val,
		"Map sector [%d,%d], which is%s your sector.  Direction?",
		(y2 / PANEL_HGT), (x2 / PANEL_WID), tmp_val);

		/* More detail */
		if (center_player)
		{
			sprintf(out_val,
			"Map sector [%d(%02d),%d(%02d)], which is%s your sector.  Direction?",
			(y2 / PANEL_HGT), (y2 % PANEL_HGT),
			(x2 / PANEL_WID), (x2 % PANEL_WID), tmp_val);
		}

		/* Assume no direction */
		dir = 0;

		/* Get a direction */
		while (!dir)
		{
			char command;

			/* Get a command (or Cancel) */
			if (!get_com(out_val, &command)) break;

			/* Extract direction */
			dir = target_dir(command);

			/* Error */
			if (!dir) bell("Illegal direction for locate!");
		}

		/* No direction */
		if (!dir) break;

		/* Apply the motion */
		y2 += (ddy[dir] * PANEL_HGT);
		x2 += (ddx[dir] * PANEL_WID);

		/* Verify the row */
		if (y2 < 0) y2 = 0;
		if (y2 > DUNGEON_HGT - SCREEN_HGT) y2 = DUNGEON_HGT - SCREEN_HGT;

		/* Verify the col */
		if (x2 < 0) x2 = 0;
		if (x2 > DUNGEON_WID - SCREEN_WID) x2 = DUNGEON_WID - SCREEN_WID;

		/* Handle "changes" */
		if ((p_ptr->wy != y2) || (p_ptr->wx != x2))
		{
			/* Update panel */
			p_ptr->wy = y2;
			p_ptr->wx = x2;

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD);

			/* Handle stuff */
			handle_stuff();
		}
	}

	/* Verify panel */
	p_ptr->update |= (PU_PANEL);

	/* Handle stuff */
	handle_stuff();
}






/*
 * The table of "symbol info" -- each entry is a string of the form
 * "X:desc" where "X" is the trigger, and "desc" is the "info".
 */
static cptr ident_info[] =
{
	" :A dark grid",
	"!:A potion",
	"\":An amulet (or necklace)",
	"#:A wall (or secret door)",
	"$:Treasure (gold or gems)",
	"%:A vein (or crack)",
	/* "&:unused", */
	"':An open door (or bridge)",
	"(:Soft armor",
	"):A shield",
	"*:A vein with treasure",
	"+:A closed/trapped door",
	",:Food (or mushroom patch)",
	"-:A wand (or rod)",
	".:Floor (or shallow water/oil)",
	"/:A polearm (Axe/Pike/etc)",
	"0:Fountain/well/altar/throne",
	"1:Entrance to General Store",
	"2:Entrance to Armory",
	"3:Entrance to Weaponsmith",
	"4:Entrance to Temple",
	"5:Entrance to Alchemy shop",
	"6:Entrance to Magic store",
	"7:Entrance to Black Market",
	"8:Entrance to your home",
	"9:Statue",
	"::Rubble (or deep water/lava/acid etc)",
	";:A glyph (or shallow lava/acid/mud)",
	"<:An up staircase (or falls/trees)",
	"=:A ring",
	">:A down staircase (or chasm)",
	"?:A scroll",
	"@:You",
	"A:Ancient Dragon/Wyrm",
	"B:Bird",
	"C:Canine",
	"D:mature Dragons",
	"E:Elemental",
	"F:Fish/Amphibian",
	"G:Ghost",
	"H:Hybrid",
	"I:Insect",
	"J:Snake",
	"K:Killer Beetle",
	"L:ghouls/Liches",
	"M:Maiar",
	"N:Nightsbane",
	"O:Ogre",
	"P:Giant",
	"Q:Quadrepeds",
	"R:Reptile",
	"S:Spider/Scorpion/Tick",
	"T:Troll",
	"U:Major Demon",
	"V:Vampire",
	"W:Wight/Wraith/etc",
	"X:Xorn/Xaren/etc",
	"Y:apes/Yeti",
	"Z:Zephyr Hound",
	"[:Hard armor",
	"\\:A hafted weapon (mace/whip/etc)",
	"]:Misc. armor",
	"^:A trap",
	"_:A staff",
	/* "`:unused", */
	"a:Ant",
	"b:Bat",
	"c:Centipede",
	"d:Young Dragon",
	"e:Floating Eye",
	"f:Feline",
	"g:Golem",
	"h:Hobbit/Dwarf",
	"i:Icky Thing",
	"j:Jelly",
	"k:Goblin",
	"l:Elf",
	"m:Mold",
	"n:Naga",
	"o:Orc",
	"p:Priest",
	"q:Mage",
	"r:Rodent",
	"s:Skeleton",
	"t:Thief/Warrior",
	"u:Minor Demon",
	"v:Vortex",
	"w:Worm/Worm-Mass",
	/* "x:unused", */
	"y:Hydra",
	"z:Zombie/Mummy",
	"{:A missile (arrow/bolt/shot)",
	"|:An edged weapon (sword/dagger/etc)",
	"}:A launcher (bow/crossbow/sling)",
	"~:A tool (or miscellaneous item)",
	NULL
};



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;


	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = l_list[w1].pkills;
		z2 = l_list[w2].pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = l_list[w1].tkills;
		z2 = l_list[w2].tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}



/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
static void roff_top(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	byte a1, a2;
	char c1, c2;


	/* Get the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Get the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

	/* A title (use "The" for non-uniques) */
	if (!(r_ptr->flags1 & (RF1_UNIQUE)))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}

	/* Dump the name */
	Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_addch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_addch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");
}


/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "mulitple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored, since they do not exist.
 */
void do_cmd_query_symbol(void)
{
	int i, n, r_idx;
	char sym, query;
	char buf[128];

	bool all = FALSE;
	bool uniq = FALSE;
	bool norm = FALSE;

	bool recall = FALSE;

	u16b why = 0;
	u16b *who;


	/* Get a character, or abort */
	if (!get_com("Enter character to be identified: ", &sym)) return;

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (sym == KTRL('A'))
	{
		all = TRUE;
		strcpy(buf, "Full monster list.");
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
		strcpy(buf, "Unique monster list.");
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
		strcpy(buf, "Non-unique monster list.");
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
	}

	/* Display the result */
	prt(buf, 0, 0);


	/* Allocate the "who" array */
	C_MAKE(who, z_info->r_max, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < z_info->r_max - 1; i++)
	{
		monster_race *r_ptr = &r_info[i];
		monster_lore *l_ptr = &l_list[i];

		/* Nothing to recall */
		if (!cheat_know && !l_ptr->sights) continue;

		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Collect "appropriate" monsters */
		if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* XXX XXX Free the "who" array */
		KILL(who);

		return;
	}


	/* Prompt */
	put_str("Recall details? (k/p/y/n): ", 0, 40);

	/* Query */
	query = inkey();

	/* Restore */
	prt(buf, 0, 0);


	/* Sort by kills (and level) */
	if (query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Sort by level */
	if (query == 'p')
	{
		why = 2;
		query = 'y';
	}

	/* Catch "escape" */
	if (query != 'y')
	{
		/* XXX XXX Free the "who" array */
		KILL(who);

		return;
	}

	/* Sort if needed */
	if (why)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	i = n - 1;

	/* Scan the monster memory */
	while (1)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Auto-recall */
		monster_race_track(r_idx);

		/* Hack -- Handle stuff */
		handle_stuff();

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");

		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Save screen */
				screen_save();

				/* Recall on screen */
				screen_roff(who[i]);

				/* Hack -- Complete the prompt (again) */
				Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");
			}

			/* Command */
			query = inkey();

			/* Unrecall */
			if (recall)
			{
				/* Load screen */
				screen_load();
			}

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}


	/* Re-display the identity */
	prt(buf, 0, 0);

	/* Free the "who" array */
	KILL(who);
}
