//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.Globalization.Collation;

namespace PhoneCall.Helpers
{
    ///////////////////////////////////////////////////
    //   This creates an alphabetized list of lists in which the items 
    //   in each list all starts with the same letter. This type is 
    //   recognized by our GridView and ListView controls and can 
    //   be grouped appropriately as well as being responsive to a
    //   SemanticZoom control for the sake of a jumplist interaction
    ///////////////////////////////////////////////////

    public class AlphaKeyGroup<T>
    {
        const string GlobeGroupKey = "\uD83C\uDF10";

        /// <summary>
        /// The Key of this group.
        /// </summary>
        public string Key { get; private set; }
        public List<T> InternalList { get; private set; }

        /// <summary>
        /// Public ctor.
        /// </summary>
        /// <param name="key">The key for this group.</param>
        public AlphaKeyGroup(string key)
        {
            Key = key;
            InternalList = new List<T>();
        }

        /// <summary>
        /// Create a list of AlphaGroup<T> with keys set by a SortedLocaleGrouping.
        /// </summary>
        /// <param name="slg">The </param>
        /// <returns>Theitems source for a LongListSelector</returns>
        private static List<AlphaKeyGroup<T>> CreateDefaultGroups(CharacterGroupings slg)
        {
            List<AlphaKeyGroup<T>> list = new List<AlphaKeyGroup<T>>();

            foreach (CharacterGrouping cg in slg)
            {
                if (cg.Label == "") continue;
                if (cg.Label == "...")
                {
                    list.Add(new AlphaKeyGroup<T>(GlobeGroupKey));
                }
                else
                {
                    list.Add(new AlphaKeyGroup<T>(cg.Label));
                }
            }

            return list;
        }

        /// <summary>
        /// Create a list of AlphaGroup<T> with keys set by a SortedLocaleGrouping.
        /// </summary>
        /// <param name="items">The items to place in the groups.</param>
        /// <param name="ci">The CultureInfo to group and sort by.</param>
        /// <param name="getKey">A delegate to get the key from an item.</param>
        /// <param name="sort">Will sort the data if true.</param>
        /// <returns>An items source for a LongListSelector</returns>
        public static List<AlphaKeyGroup<T>> CreateGroups(IEnumerable<T> items, Func<T, string> keySelector, bool sort)
        {
            CharacterGroupings slg = new CharacterGroupings();
            List<AlphaKeyGroup<T>> list = CreateDefaultGroups(slg);

            foreach (T item in items)
            {
                int index = 0;
                {
                    string label = slg.Lookup(keySelector(item));
                    index = list.FindIndex(alphakeygroup => (alphakeygroup.Key.Equals(label, StringComparison.CurrentCulture)));
                }

                if (index >= 0 && index < list.Count)
                {
                    list[index].InternalList.Add(item);
                }
            }

            if (sort)
            {
                foreach (AlphaKeyGroup<T> group in list)
                {
                    group.InternalList.Sort((c0, c1) => { return keySelector(c0).CompareTo(keySelector(c0)); });
                }
            }

            return list;
        }
    }
}