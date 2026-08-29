# Dark Arisen writing guardrails

These rules apply to new dialogue, journal text, quest prose, lore, item text and AI-assisted writing.

## Source first

Do not invent story material to make a content count look complete. If the governing documents do not define enough of a quest, character, place or outcome, leave it unwritten and record the gap.

Existing authored lines are evidence of voice. Preserve their rhythm and vocabulary rather than smoothing them into a shared house style.

## People should not sound alike

A line should still make sense as belonging to its speaker if the name tag is removed.

Use the character's established education, work, region, age, habits and relationship to Jake. People may be blunt, repetitive, evasive, awkward, funny at the wrong time, or stop before finishing a thought. Do not improve those traits away.

Avoid making everyone articulate the theme of the scene. Most people talk about the thing in front of them.

## Avoid machine-shaped prose

Do not default to:

- three-part rhetorical lists,
- balanced oppositions in every paragraph,
- repeated constructions such as “not X, but Y”,
- generic solemnity,
- constant metaphors about weight, ghosts, scars, echoes, darkness or memory unless that speaker already uses them,
- dialogue in which every sentence is quotable,
- exposition disguised as banter,
- identical sentence length and punctuation across speakers,
- modern therapy language unless the character/source genuinely supports it,
- invented dialect spelling used as decoration.

Short, ordinary lines are allowed. Silence is allowed. A character can answer the wrong part of a question.

## Dialogue test

Before accepting a new line, check:

1. Is the information actually authored?
2. Would this specific person say it this way?
3. Is the line doing work a gesture, pause, object or environment could do instead?
4. Does it explain something the player already understands?
5. Does it sound cleaner or wiser than a real person in that situation would sound?

If 1 fails, do not write the line. If 2 fails, rewrite it. If 3–5 are true, cut or roughen it.

## Quest and journal text

Jake's notebook is not a quest UI. It should read like something he decided was worth writing down, not an objective list.

Do not add fabricated directions, reminders, urgency, completion language or task verbs when the source does not provide them.

The neutral IDs in `Docs/QUEST_CONTENT_GAPS.md` are bookkeeping only. They must never be turned into quest premises by an AI model.

## AI-assisted production

Kimi/Game Studios may help with spatial layouts, implementation breakdowns, continuity checks, animation coverage, asset lists and QA. For prose it is a draft assistant only.

It may not:

- fill missing Turn or Standing slots,
- invent named NPCs or histories,
- create a new relationship beat,
- invent rewards or endings,
- create final dialogue for a speaker without enough voice-source material.

Any prose proposal must cite the governing character/region/quest source. If it cannot, mark `DESIGN-GAP` and stop there.
